#include <iostream>
#include "mpi.h"

void Print_Matrix(int* Mat, int size)
{
    std::cout << "Multiplication result: " << std::endl;
    for (int i = 0; i < size; i++)
    {
        for (int j = 0; j < size; j++)
        {
            std::cout << Mat[i * size + j] << " ";
        }
        std::cout << std::endl;
    }
}

int main(int argc, char *argv[])
{
    double start, stop;
    int i, j, k, l;
    int *A, *B, *C, *buffer, *ans;
    int size = (argc > 1) ? atoi(argv[1]) : 1000;  // Размер матрицы (по умолчанию 1000)
    int rank, numprocs, line;

    MPI_Init(&argc, &argv);  // Инициализация MPI
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);  // Получить текущий номер процесса
    MPI_Comm_size(MPI_COMM_WORLD, &numprocs);  // Получить количество процессов

    line = size / numprocs;  // Делим данные на блоки (количество процессов), основной процесс также должен обрабатывать данные
    A = new int[size * size];
    B = new int[size * size];
    C = new int[size * size];
    buffer = new int[size * line];  // Размер пакета данных
    ans = new int[size * line];  // Сохраняем результат расчета блока данных

    // Основной процесс присваивает матрице начальное значение и передает матрицу B другим процессам
    if (rank == 0)
    {
        // Инициализация матрицы A
        for (i = 0; i < size; i++)
            for (j = 0; j < size; j++)
                A[i * size + j] = (i + 2) * (j + 1);

        // Инициализация матрицы B
        for (i = 0; i < size; i++)
            for (j = 0; j < size; j++)
                B[i * size + j] = (i + 1) + 2 * (j + 3);

        start = MPI_Wtime();  // Время начала

        // Отправляем матрицу B другим процессам
        for (i = 1; i < numprocs; i++) {
            MPI_Send(B, size * size, MPI_INT, i, 0, MPI_COMM_WORLD);
        }

        // Отправляем части матрицы A другим процессам
        for (l = 1; l < numprocs; l++) {
            MPI_Send(A + (l - 1) * line * size, size * line, MPI_INT, l, 1, MPI_COMM_WORLD);
        }

        // Получаем результаты от других процессов
        for (k = 1; k < numprocs; k++) {
            MPI_Recv(ans, line * size, MPI_INT, k, 3, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            // Записываем результаты в матрицу C
            for (i = 0; i < line; i++) {
                for (j = 0; j < size; j++) {
                    C[((k - 1) * line + i) * size + j] = ans[i * size + j];
                }
            }
        }

        // Вычисляем оставшиеся элементы на основном процессе
        for (i = (numprocs - 1) * line; i < size; i++) {
            for (j = 0; j < size; j++) {
                int temp = 0;
                for (k = 0; k < size; k++)
                    temp += A[i * size + k] * B[k * size + j];
                C[i * size + j] = temp;
            }
        }

        stop = MPI_Wtime();  // Время завершения

        // Выводим время выполнения в консоль
        std::cout << "Rank: " << rank << "\nTime: " << (stop - start) << " seconds" << std::endl;
    }

        // Другие процессы получают данные, выполняют вычисления и отправляют результат обратно на основной процесс
    else {
        // Получаем матрицу B от основного процесса
        MPI_Recv(B, size * size, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        // Получаем часть матрицы A
        MPI_Recv(buffer, size * line, MPI_INT, 0, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        // Выполняем умножение и отправляем результат обратно
        for (i = 0; i < line; i++) {
            for (j = 0; j < size; j++) {
                int temp = 0;
                for (k = 0; k < size; k++)
                    temp += buffer[i * size + k] * B[k * size + j];
                ans[i * size + j] = temp;
            }
        }

        // Отправляем результат на основной процесс
        MPI_Send(ans, line * size, MPI_INT, 0, 3, MPI_COMM_WORLD);
    }

    // Завершаем MPI
    MPI_Finalize();

    return 0;
}
