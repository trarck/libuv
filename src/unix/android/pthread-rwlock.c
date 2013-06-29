#include	"uv.h"
#include	<errno.h>
#if defined(ANDROID_PLATFORM) && ANDROID_PLATFORM<9
int
pthread_rwlock_init(pthread_rwlock_t *rw, pthread_rwlockattr_t *attr)
{
	int		result;

	if (attr != NULL)
		return(EINVAL);		/* not supported */

	if ( (result = pthread_mutex_init(&rw->rw_mutex, NULL)) != 0)
		goto err1;
	if ( (result = pthread_cond_init(&rw->rw_condreaders, NULL)) != 0)
		goto err2;
	if ( (result = pthread_cond_init(&rw->rw_condwriters, NULL)) != 0)
		goto err3;
	rw->rw_nwaitreaders = 0;
	rw->rw_nwaitwriters = 0;
	rw->rw_refcount = 0;
	rw->rw_magic = RW_MAGIC;

	return(0);

err3:
	pthread_cond_destroy(&rw->rw_condreaders);
err2:
	pthread_mutex_destroy(&rw->rw_mutex);
err1:
	return(result);
}
/* end init */

int
pthread_rwlock_destroy(pthread_rwlock_t *rw)
{
	if (rw->rw_magic != RW_MAGIC)
		return(EINVAL);
	if (rw->rw_refcount != 0 ||
		rw->rw_nwaitreaders != 0 || rw->rw_nwaitwriters != 0)
		return(EBUSY);

	pthread_mutex_destroy(&rw->rw_mutex);
	pthread_cond_destroy(&rw->rw_condreaders);
	pthread_cond_destroy(&rw->rw_condwriters);
	rw->rw_magic = 0;

	return(0);
}
/* end destroy */

/* include rwlock_cancelrdwait */
static void
rwlock_cancelrdwait(void *arg)
{
	pthread_rwlock_t	*rw;

	rw = arg;
	rw->rw_nwaitreaders--;
	pthread_mutex_unlock(&rw->rw_mutex);
}
/* end rwlock_cancelrdwait */

int
pthread_rwlock_rdlock(pthread_rwlock_t *rw)
{
	int		result;

	if (rw->rw_magic != RW_MAGIC)
		return(EINVAL);

	if ( (result = pthread_mutex_lock(&rw->rw_mutex)) != 0)
		return(result);

		/* 4give preference to waiting writers */
	while (rw->rw_refcount < 0 || rw->rw_nwaitwriters > 0) {
		rw->rw_nwaitreaders++;
		pthread_cleanup_push(rwlock_cancelrdwait, (void *) rw);
		result = pthread_cond_wait(&rw->rw_condreaders, &rw->rw_mutex);
		pthread_cleanup_pop(0);
		rw->rw_nwaitreaders--;
		if (result != 0)
			break;
	}
	if (result == 0)
		rw->rw_refcount++;		/* another reader has a read lock */

	pthread_mutex_unlock(&rw->rw_mutex);
	return (0);
}

int
pthread_rwlock_tryrdlock(pthread_rwlock_t *rw)
{
	int		result;

	if (rw->rw_magic != RW_MAGIC)
		return(EINVAL);

	if ( (result = pthread_mutex_lock(&rw->rw_mutex)) != 0)
		return(result);

	if (rw->rw_refcount == -1 || rw->rw_nwaitwriters > 0)
		result = EBUSY;			/* held by a writer or waiting writers */
	else
		rw->rw_refcount++;		/* increment count of reader locks */

	pthread_mutex_unlock(&rw->rw_mutex);
	return(result);
}
/* end tryrdlock */

/* include rwlock_cancelwrwait */
static void
rwlock_cancelwrwait(void *arg)
{
	pthread_rwlock_t	*rw;

	rw = arg;
	rw->rw_nwaitwriters--;
	pthread_mutex_unlock(&rw->rw_mutex);
}
/* end rwlock_cancelwrwait */

int
pthread_rwlock_wrlock(pthread_rwlock_t *rw)
{
	int		result;

	if (rw->rw_magic != RW_MAGIC)
		return(EINVAL);

	if ( (result = pthread_mutex_lock(&rw->rw_mutex)) == -1)
		return(result);

	while (rw->rw_refcount != 0) {
		rw->rw_nwaitwriters++;
		pthread_cleanup_push(rwlock_cancelwrwait, (void *) rw);
		result = pthread_cond_wait(&rw->rw_condwriters, &rw->rw_mutex);
		pthread_cleanup_pop(0);
		rw->rw_nwaitwriters--;
		if (result != 0)
			break;
	}
	if (result == 0)
		rw->rw_refcount = -1;

	pthread_mutex_unlock(&rw->rw_mutex);
	return(0);
}
/* end func */

int
pthread_rwlock_trywrlock(pthread_rwlock_t *rw)
{
	int		result;

	if (rw->rw_magic != RW_MAGIC)
		return(EINVAL);

	if ( (result = pthread_mutex_lock(&rw->rw_mutex)) != 0)
		return(result);

	if (rw->rw_refcount != 0)
		result = EBUSY;			/* held by either writer or reader(s) */
	else
		rw->rw_refcount = -1;	/* available, indicate a writer has it */

	pthread_mutex_unlock(&rw->rw_mutex);
	return(result);
}
/* end trywrlock */

int
pthread_rwlock_unlock(pthread_rwlock_t *rw)
{
	int		result;

	if (rw->rw_magic != RW_MAGIC)
		return(EINVAL);

	if ( (result = pthread_mutex_lock(&rw->rw_mutex)) != 0)
		return(result);

	if (rw->rw_refcount > 0)
		rw->rw_refcount--;			/* releasing a reader */
	else if (rw->rw_refcount == -1)
		rw->rw_refcount = 0;		/* releasing a reader */
	else{
		//printf("rw_refcount = %d", rw->rw_refcount);
    }

		/* 4give preference to waiting writers over waiting readers */
	if (rw->rw_nwaitwriters > 0)
		result = pthread_cond_signal(&rw->rw_condwriters);
	else if (rw->rw_nwaitreaders > 0)
		result = pthread_cond_broadcast(&rw->rw_condreaders);

	pthread_mutex_unlock(&rw->rw_mutex);
	return(result);
}
/* end unlock */
#endif
