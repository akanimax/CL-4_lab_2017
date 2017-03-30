#include <iostream>
#include <omp.h>

#define MAX_SIZE 50 // Maxsize of the array 

using namespace std;

class q_sort {
private: 
	int arr[MAX_SIZE];

public:
	void read_array(int);
	void display_array(int);
	int partition(int, int);
	void quicksort(int, int);
};

void q_sort::read_array(int n) {
	cout << "\nReading the elements from the user: " << endl;
	cout << "\nEnter the Elements (space separated): ";
	
	for(int i = 0; i < n; i++)
		cin >> arr[i];
}

void q_sort::display_array(int n) {

	cout<<"\nThe sorted elements are : \n";
	
	for(int i = 0;i < n; i++) {
		cout << arr[i] << "  ";
    } cout << endl;
}

int q_sort::partition(int p, int r) {
	int lt[r-p], gt[r-p], i, j;
	
	int pivot = arr[r];
	int lt_n = 0;
	int gt_n = 0;

	#pragma omp parallel for
    for(i = p; i < r; i++) {
        if(arr[i] < arr[r]) {
			lt[lt_n++] = arr[i];
		}
		
		else {
        	gt[gt_n++] = arr[i];
        }
    }	
    
	for(i = 0; i < lt_n; i++) {
		arr[p + i] = lt[i];
    } arr[p + lt_n] = pivot;
	

	for(j = 0; j < gt_n; j++) {
		arr[p + lt_n + j + 1] = gt[j];
    } 
	
	return p + lt_n;
}

void q_sort::quicksort(int p, int r) {
	int div;
	
	if(p < r) { 
		div = partition(p,r); 
		
		cout << "\nforking into two parallel threads: " << endl;
		#pragma omp parallel sections
        {   
			#pragma omp section 
			{
				cout << "Sorting the left half: " << endl;
				quicksort(p, div - 1);
			} 
			
			#pragma omp section 
			{
				cout << "Sorting the right half: " << endl;
				quicksort(div + 1, r);
			} 
        }
    }
}

int main() {
    q_sort q;
    int n;
	
	cout<<"Enter the number of elements : ";
	cin>>n;
	
	q.read_array(n);
	q.quicksort(0,n-1);
	q.display_array(n);
    return 0;
}

