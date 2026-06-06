#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#define N 5

// Mỗi cây đũa là một Semaphore
sem_t chopstick[N]; 
// Semaphore giới hạn tối đa 4 triết gia cùng vào bàn lấy đũa để tránh deadlock
sem_t room;         

void *philosopher(void*);
void eat(int);
void think(int);

int main()
{
   int i;
   pthread_t tid[N];
   int phil_id[N]; // Dùng mảng riêng để lưu ID, tránh lỗi ghi đè dữ liệu khi truyền pointer

   // Khởi tạo semaphore cho phòng ăn (tối đa 4 người)
   sem_init(&room, 0, 4);
   
   // Khởi tạo semaphore cho từng cây đũa (giá trị là 1 - tự do)
   for (i = 0; i < N; i++)
      sem_init(&chopstick[i], 0, 1);

   // Tạo các thread triết gia
   for (i = 0; i < N; i++)
   {
      phil_id[i] = i;
      pthread_create(&tid[i], NULL, philosopher, (void*) &phil_id[i]);
   }

   // Chờ các thread (thực tế vòng lặp while(1) sẽ chạy vô tận)
   for (i = 0; i < N; i++)
      pthread_join(tid[i], NULL);

   // Hủy semaphore khi kết thúc (nếu có)
   sem_destroy(&room);
   for (i = 0; i < N; i++)
      sem_destroy(&chopstick[i]);

   return 0;
}

void *philosopher(void *num)
{
   int phil = *(int*) num;
   printf("Philosopher %d has entered room\n", phil);
     
   while (1)
   {
      // Thử nghĩ một lúc trước khi đói
      think(phil);
      sleep(1);

      // Đi vào phòng ăn (giới hạn tối đa 4 người)
      sem_wait(&room);

      // Lấy đũa bên trái
      sem_wait(&chopstick[phil]);
      // Lấy đũa bên phải
      sem_wait(&chopstick[(phil + 1) % N]);
      
      printf("Philosopher %d takes fork %d and %d\n", phil, phil, (phil + 1) % N);
           
      // Ăn
      eat(phil);
      sleep(2);

      printf("Philosopher %d puts fork %d and %d down\n", phil, phil, (phil + 1) % N);
      
      // Buông đũa bên phải và bên trái
      sem_post(&chopstick[(phil + 1) % N]);
      sem_post(&chopstick[phil]);

      // Rời khỏi vị trí ăn để nhường cho người thứ 5 (nếu có ai đang đợi)
      sem_post(&room);
   }
}

void eat(int phil)
{
   printf("Philosopher %d is eating\n", phil);
}

void think(int phil)
{
   printf("Philosopher %d is thinking\n", phil);
}