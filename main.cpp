#include <iostream>
#include <cmath>
#include <limits>
#include <chrono>
using namespace std;
struct Point {
    double x, y;
};
double euclidean_distance(const Point &a, const Point &b) {
    double dx = a.x - b.x;
    double dy = a.y - b.y;
    return sqrt(dx * dx + dy * dy);
}
double** compute_distance_matrix(Point* points, int n) {
    double** dist = new double*[n];
    for (int i = 0; i < n; i++) {
        dist[i] = new double[n];
    }
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            if (i == j)
                dist[i][j] = 0.0;
            else
                dist[i][j] = euclidean_distance(points[i], points[j]);
        }
    }
    return dist;
}
int* greedy_tour(double** dist, int n) {
    bool* visited = new bool[n];
    for (int i = 0; i < n; i++) {
        visited[i] = false;
    }
    int* tour = new int[n];
    tour[0] = 0;
    visited[0] = true;
    int current = 0;
    for (int i = 1; i < n; i++) {
        int next = -1;
        double best = numeric_limits<double>::max();
        for (int j = 0; j < n; j++) {
            if (!visited[j] && dist[current][j] < best) {
                best = dist[current][j];
                next = j;
            }
        }
        tour[i] = next;
        visited[next] = true;
        current = next;
    }
    delete [] visited;
    return tour;
}
double compute_length(int* tour, double** dist, int n, bool cycle) {
    double length = 0.0;
    for (int i = 0; i < n - 1; i++) {
        length += dist[tour[i]][tour[i + 1]];
    }
    if (cycle)
        length += dist[tour[n - 1]][tour[0]];
    return length;
}
void reverse_segment(int* arr, int start, int end) {
    while (start < end) {
        int temp = arr[start];
        arr[start] = arr[end];
        arr[end] = temp;
        start++;
        end--;
    }
}
bool two_opt_swap(int* tour, double** dist, int n) {
    double oldLength = compute_length(tour, dist, n, true);
    for (int i = 0; i < n - 1; i++) {
        for (int k = i + 1; k < n; k++) {
            int* new_tour = new int[n];
            for (int m = 0; m < n; m++) {
                new_tour[m] = tour[m];
            }
            reverse_segment(new_tour, i + 1, k);
            double newLength = compute_length(new_tour, dist, n, true);
            if (newLength < oldLength) {
                for (int m = 0; m < n; m++) {
                    tour[m] = new_tour[m];
                }
                delete [] new_tour;
                return true;
            }
            delete [] new_tour;
        }
    }
    return false;
}
bool three_opt_swap(int* tour, double** dist, int n) {
    double oldLength = compute_length(tour, dist, n, true);
    for (int i = 0; i < n - 2; i++) {
        for (int j = i + 1; j < n - 1; j++) {
            for (int k = j + 1; k < n; k++) {
                for (int candidate = 1; candidate <= 4; candidate++) {
                    int* new_tour = new int[n];
                    int idx = 0;
                    for (int m = 0; m <= i; m++) {
                        new_tour[idx++] = tour[m];
                    }
                    if (candidate == 1) {
                        for (int m = j; m >= i + 1; m--) {
                            new_tour[idx++] = tour[m];
                        }
                        for (int m = j + 1; m <= k; m++) {
                            new_tour[idx++] = tour[m];
                        }
                    } else if (candidate == 2) {
                        for (int m = i + 1; m <= j; m++) {
                            new_tour[idx++] = tour[m];
                        }
                        for (int m = k; m >= j + 1; m--) {
                            new_tour[idx++] = tour[m];
                        }
                    } else if (candidate == 3) {
                        for (int m = j; m >= i + 1; m--) {
                            new_tour[idx++] = tour[m];
                        }
                        for (int m = k; m >= j + 1; m--) {
                            new_tour[idx++] = tour[m];
                        }
                    } else if (candidate == 4) {
                        for (int m = j + 1; m <= k; m++) {
                            new_tour[idx++] = tour[m];
                        }
                        for (int m = i + 1; m <= j; m++) {
                            new_tour[idx++] = tour[m];
                        }
                    }
                    for (int m = k + 1; m < n; m++) {
                        new_tour[idx++] = tour[m];
                    }
                    double newLength = compute_length(new_tour, dist, n, true);
                    if (newLength < oldLength) {
                        for (int m = 0; m < n; m++) {
                            tour[m] = new_tour[m];
                        }
                        delete [] new_tour;
                        return true;
                    }
                    delete [] new_tour;
                }
            }
        }
    }
    return false;
}
long long measure_time(void (*func)()) {
    auto start = chrono::high_resolution_clock::now();
    func();
    auto end = chrono::high_resolution_clock::now();
    return chrono::duration_cast<chrono::milliseconds>(end - start).count();
}
int main() {
    int n;
    cout << "Enter number of points: ";
    cin >> n;
    Point* points = new Point[n];
    cout << "Enter coordinates (x y) for each point:" << endl;
    for (int i = 0; i < n; i++) {
        cin >> points[i].x >> points[i].y;
    }
    int mode;
    cout << "Enter 0 for path, 1 for cycle: ";
    cin >> mode;
    bool is_cycle = (mode == 1);
    double** dist = compute_distance_matrix(points, n);
    int* greedyTour = greedy_tour(dist, n);
    cout << "\nGreedy (nearest neighbor) tour: ";
    for (int i = 0; i < n; i++) {
        cout << greedyTour[i] + 1 << " ";
    }
    cout << "\nLength = " << compute_length(greedyTour, dist, n, is_cycle) << "\n\n";
    int* tour_two_opt = new int[n];
    for (int i = 0; i < n; i++) {
        tour_two_opt[i] = greedyTour[i];
    }
    auto start_two = chrono::high_resolution_clock::now();
    while (two_opt_swap(tour_two_opt, dist, n)) { }
    auto end_two = chrono::high_resolution_clock::now();
    chrono::duration<double> elapsed_two = end_two - start_two;
    double length_two = compute_length(tour_two_opt, dist, n, is_cycle);
    cout << "2-opt optimization:" << endl;
    cout << "Time taken: " << elapsed_two.count() << " seconds" << endl;
    cout << "Tour length: " << length_two << endl;
    cout << "Tour: ";
    for (int i = 0; i < n; i++) {
        cout << tour_two_opt[i] + 1 << " ";
    }
    cout << "\n\n";
    int* tour_three_opt = new int[n];
    for (int i = 0; i < n; i++) {
        tour_three_opt[i] = greedyTour[i];
    }
    auto start_three = chrono::high_resolution_clock::now();
    while (three_opt_swap(tour_three_opt, dist, n)) { }
    auto end_three = chrono::high_resolution_clock::now();
    chrono::duration<double> elapsed_three = end_three - start_three;
    double length_three = compute_length(tour_three_opt, dist, n, is_cycle);
    cout << "3-opt optimization:" << endl;
    cout << "Time taken: " << elapsed_three.count() << " seconds" << endl;
    cout << "Tour length: " << length_three << endl;
    cout << "Tour: ";
    for (int i = 0; i < n; i++) {
        cout << tour_three_opt[i] + 1 << " ";
    }
    cout << "\n";
    delete [] greedyTour;
    delete [] tour_two_opt;
    delete [] tour_three_opt;
    delete [] points;
    for (int i = 0; i < n; i++) {
        delete [] dist[i];
    }
    delete [] dist;
    return 0;
}
