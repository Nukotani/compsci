#include <limits.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <x86intrin.h>

#define LEFT 0
#define RIGHT 1

static inline void print_arr(int *arr, int size) {
  for (int i = 0; i < size; ++i) {
    printf("%d", arr[i]);
  }
  printf("\n");
}

static inline void swap(int *arr, int x, int y) {
  int _temp = arr[x];
  arr[x] = arr[y];
  arr[y] = _temp;
}

void randomize(int *arr, int size) {
  int random;
  int rand_floor;

  for (int i = size - 1; i >= 0; --i) { // iterate from the last index
    rand_floor =
        floor(i * ((double)rand() /
                   (double)((unsigned)RAND_MAX +
                            1))); // find a random number between 0 and i
    swap(arr, i, rand_floor);     // swap arr[i] with a value in a random index
                                  // (including arr[i] itself)
  }
}

void rotate(int *arr, int size, int mode) {
  int mode_bit =
      mode * ~(0); // 0x0 for left rotation and 0xFFFFFFFF for left rotation
  int arr_end = size - 1; // the last index
  int i_inc4 =
      4 - (mode << 3); // -4 for right rotation or 4 for left rotation
  int i_inc1 = 1 - (mode << 1); // -1 for right roration or 1 for left rotation

  const int aligned_size4 =
      arr_end - (arr_end & 3); // align the size to a multiple of 4

  int _temp =
      arr[arr_end & mode_bit]; // the first value of the array for left rotation
                               // and the last for right rotation

  for (int i =
           1 + ((arr_end - 5) & mode_bit); // 1 for left rotation and the second
                                           // last index for right rotation;
       (i < aligned_size4) && (i >= 0); i += i_inc4) {

    _mm_storeu_si128((__m128i *)&arr[i - i_inc1],
                     _mm_loadu_si128((__m128i *)&arr[i]));
  }

  for (int i = (arr_end & mode_bit) + (i_inc1 *(aligned_size4 + 1)); (i < size) && (i >= 0); i += i_inc1) {
	  arr[i - i_inc1] = arr[i];
  }
  arr[arr_end & ~mode_bit] = _temp;
}

void reverse(int *arr, int size) {
  int _temp = 0;
  int max_index = size - 1;
  int right_index = 0;
  for (int i = 0; i < (size >> 1); ++i) {
    right_index = max_index - i;
    swap(arr, i, right_index);
  }
}

void insert(int *arr, int size, int input, int index) {
  int diff = index - input;
  int new_size = size + 1;
  int new_arr[new_size];

  for (int i = 0; i < index; ++i) {
    new_arr[i] = arr[i];
  }

  new_arr[index] = input;

  for (int i = index; i < size; i++) {
    new_arr[i + 1] = arr[i];
  }

  memcpy((void *)arr, (void *)new_arr, sizeof(int) * 6);
}

void sum(int *arr1, int *arr2, int size, int *result) {
  const int aligned_size =
      size - (size & 3); // align the size to a multiple of 4

  for (int i = 0; i < aligned_size;
       i += 4) { // calculate the sum of 4 values / indices at once
    _mm_storeu_si128((__m128i *)&result[i],
                     _mm_add_epi32(_mm_loadu_si128((__m128i *)&arr1[i]),
                                   _mm_loadu_si128((__m128i *)&arr2[i])));
  }

  for (int i = aligned_size; i < size; ++i) { // calculate the rest
    result[i] = arr1[i] + arr2[i];
  }
}
int main() {
  int arr[5] = {0, 1, 2, 3, 4};

  rotate(arr, 5, RIGHT);
  print_arr(arr, 5);

  insert(arr, 5, 3, 2);
  print_arr(arr, 6);

  rotate(arr, 6, LEFT);
  print_arr(arr, 6);

  randomize(arr, 6);
  print_arr(arr, 6);

  reverse(arr, 6);
  print_arr(arr, 6);

  int arr1[6] = {0, 1, 2, 3, 4, 5};
  int result[6];
  sum(arr, arr1, 6, result);
  print_arr(result, 6);
}
