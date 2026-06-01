#ifndef PTHREAD_H
#include <pthread.h>
#endif

typedef struct pthread_seqlock { /* TODO: implement the structure */
   unsigned int sequence; //số thứ tự của writer, odd: đang viết, even: không có writer
   int reader_count; //số lượng reader đang trong vùng tranh chấp
   pthread_mutex_t mutex; //mutex để bảo vệ reader_count
   pthread_cond_t cond; //condition variable để báo cho writer khi reader_count = 0
   pthread_mutex_t write_mutex; //mutex để bảo vệ writer, đảm bảo chỉ có 1 writer được phép viết tại 1 thời điểm
} pthread_seqlock_t;

static inline void pthread_seqlock_init(pthread_seqlock_t *rw)
{
   /* TODO: ... */
   rw->sequence = 0;
   rw->reader_count = 0;
   pthread_mutex_init(&rw->mutex, NULL);
   pthread_mutex_init(&rw->write_mutex, NULL);
   pthread_cond_init(&rw->cond, NULL);
}

static inline void pthread_seqlock_destroy(pthread_seqlock_t *rw)
{
   pthread_mutex_destroy(&rw->mutex);
   pthread_mutex_destroy(&rw->write_mutex);
   pthread_cond_destroy(&rw->cond);
}

static inline void pthread_seqlock_wrlock(pthread_seqlock_t *rw) // bắt đầu ghi
{
   /* TODO: ... */
   pthread_mutex_lock(&rw->write_mutex); //đảm bảo chỉ có 1 writer được phép viết tại 1 thời điểm
   pthread_mutex_lock(&rw->mutex); 
   rw->sequence++;
   pthread_mutex_unlock(&rw->mutex);
}

static inline void pthread_seqlock_wrunlock(pthread_seqlock_t *rw) // sau khi ghi xong
{
   /* TODO: ... */
   pthread_mutex_lock(&rw->mutex);
   rw->sequence++;
   pthread_cond_broadcast(&rw->cond); //báo cho writer khi reader_count = 0
   pthread_mutex_unlock(&rw->mutex);
   pthread_mutex_unlock(&rw->write_mutex);
}

static inline unsigned pthread_seqlock_rdlock(pthread_seqlock_t *rw) // bắt đầu đọc
{
   /* TODO: ... */
   unsigned int seq;
   pthread_mutex_lock(&rw->mutex);
   while (rw->sequence % 2 == 1) { //nếu có writer đang viết thì phải đợi
      pthread_cond_wait(&rw->cond, &rw->mutex);
   }
   rw->reader_count++; //tăng số lượng reader đang trong vùng tranh chấp
   seq = rw->sequence; //lấy số thứ tự của writer hiện tại
   pthread_mutex_unlock(&rw->mutex);
   return seq;
}


static inline unsigned pthread_seqlock_rdunlock(pthread_seqlock_t *rw) // sau khi đọc xong
{
   /* TODO: ... */
   pthread_mutex_lock(&rw->mutex);
   rw->reader_count--; //giảm số lượng reader đang trong vùng tranh chấp
   pthread_mutex_unlock(&rw->mutex);
   return 0;
}

