// Fuente GeeksforGeeks: 3-Way QuickSort (Dutch National Flag)
// URL: https://www.geeksforgeeks.org/dsa/3-way-quicksort-dutch-national-flag/
//

#include <bits/stdc++.h>
using namespace std;

/* This function partitions a[] in three parts
   a) a[l..i] contains all elements smaller than pivot
   b) a[i+1..j-1] contains all occurrences of pivot
   c) a[j..r] contains all elements greater than pivot */
void partition(std::vector<int>& a, int l, int r, int &i, int &j)
{
    int randIdx = l + rand() % (r - l + 1);
    swap(a[randIdx], a[r]);

    i = l - 1, j = r;
    int p = l - 1, q = r;
    int v = a[r];

    while (true) {
        // From left, find the first element greater than
        // or equal to v. This loop will definitely
        // terminate as v is last element
        while (a[++i] < v)
            ;

        // From right, find the first element smaller than
        // or equal to v
        while (v < a[--j])
            if (j == l)
                break;

        // If i and j cross, then we are done
        if (i >= j)
            break;

        // Swap, so that smaller goes on left greater goes
        // on right
        swap(a[i], a[j]);

        // Move all same left occurrence of pivot to
        // beginning of array and keep count using p
        if (a[i] == v) {
            p++;
            swap(a[p], a[i]);
        }

        // Move all same right occurrence of pivot to end of
        // array and keep count using q
        if (a[j] == v) {
            q--;
            swap(a[j], a[q]);
        }
    }

    // Move pivot element to its correct index
    swap(a[i], a[r]);

    // Move all left same occurrences from beginning
    // to adjacent to arr[i]
    j = i - 1;
    for (int k = l; k < p; k++, j--)
        swap(a[k], a[j]);

    // Move all right same occurrences from end
    // to adjacent to arr[i]
    i = i + 1;
    for (int k = r - 1; k > q; k--, i++)
        swap(a[i], a[k]);
}

// 3-way partition based quick sort
void quickSort(std::vector<int>& a, int l, int r)
{while (l < r) {
        int i, j;

        // Note that i and j are passed as reference
        partition(a, l, r, i, j);

        // j es el final de los menores, i es el inicio de los mayores.
        // Hacemos recursión SOLO en la mitad más pequeña:
        if ((j - l) < (r - i)) {
            quickSort(a, l, j); // Recursión en la izquierda
            l = i;              // El bucle while procesa la derecha
        } else {
            quickSort(a, i, r); // Recursión en la derecha
            r = j;              // El bucle while procesa la izquierda
        }
    }
}