#include <windows.h>
#include <iostream>
#include <iomanip>
#include <vector>
#include <execution>
#include <numeric>
#include <memory>
#include <random>
#include <chrono>
#include <omp.h>
#define _USE_MATH_DEFINES
#include <math.h>
#include <conio.h>

#include "Parametrized3DCurveDLL.h"

using namespace std;

typedef acHorizontalCircle * (*curve_dll_circle_type)(float, float, float);
typedef acHorizontalEllipse* (*curve_dll_ellipse_type)(float, float, float, float);
typedef acVertical3DHelix* (*curve_dll_helix_type)(float, float, float, float);

curve_dll_circle_type   СreateHorizontalCircle;
curve_dll_ellipse_type  СreateHorizontalEllipse;
curve_dll_helix_type    СreateVertical3DHelix;

// generate a vector with curves
vector< shared_ptr<cParametrized3DCurve> > make_curves(int num_curves)
{
    mt19937 gen(65535);
    vector< shared_ptr<cParametrized3DCurve> > result_vector;

    // using int distibutions for float vectors as a demonstration
    // because with int values it's easier to verify the result manually
    uniform_int_distribution<int> curve_type_distribution(0, 2);
    uniform_int_distribution<int> curve_x0_distribution(-10, 10);
    uniform_int_distribution<int> curve_y0_distribution(-10, 10);
    uniform_int_distribution<int> curve_radius_distribution(1, 10);
    uniform_int_distribution<int> curve_radiusx_distribution(1, 10);
    uniform_int_distribution<int> curve_radiusy_distribution(1, 10);
    uniform_int_distribution<int> curve_step_distribution(-10, 10);

    for (int i = 0; i < num_curves; i++) {
        int curve_type = curve_type_distribution(gen);
        if (curve_type == 0) {
            result_vector.push_back(shared_ptr<acHorizontalCircle>(
                    СreateHorizontalCircle( curve_x0_distribution(gen),
                                            curve_y0_distribution(gen),
                                            curve_radius_distribution(gen)
                    )
                ));
        }
        else if (curve_type == 1) {
            result_vector.push_back(shared_ptr<acHorizontalEllipse>(
                СreateHorizontalEllipse(    curve_x0_distribution(gen),
                                            curve_y0_distribution(gen),
                                            curve_radiusx_distribution(gen),
                                            curve_radiusy_distribution(gen)
                )
            ));
        }
        else if (curve_type == 2) {
            result_vector.push_back(shared_ptr<acVertical3DHelix>(
                СreateVertical3DHelix(      curve_x0_distribution(gen),
                                            curve_y0_distribution(gen),
                                            curve_radius_distribution(gen),
                                            curve_step_distribution(gen)
                )
            ));
        }
    }
    return result_vector;
}

// print a table with curve data from a vector
template<class CurveType>
void print_curves(const vector< shared_ptr<CurveType> >& curves, float t = NAN, int precision = 2) {
    vector<string> titles = { "Type", "x0", "y0", "r1", "r2/step", "x(t)", "y(t)", "z(t)", "x'(t)", "y'(t)", "z'(t)" };
    vector<int> column_sizes = { 12,     8,    8,    8,    16,        8,      8,      16,      8,      8,       8 };
    int num_columns = 11;
    int point_column = 5;
    int derivative_column = 8;

    vector<int> positions(column_sizes.size(), 0);
    partial_sum(column_sizes.begin(), column_sizes.end() - 1, positions.begin() + 1);
    vector<float> values;
    stringstream string_buffer;
    string_buffer << fixed << setprecision(precision);
    int position_shift;
    if (!isnan(t)) {
        string_buffer << "t = " << t;
        cout << string_buffer.str() << endl;
    }

    string_buffer.str("");
    for (int i = 0; i < num_columns; i++) {
        position_shift = positions[i] - string_buffer.str().length();
        if (position_shift > 0)
            string_buffer << string(position_shift, ' ');
        string_buffer << titles[i];
        if (isnan(t) && (i > 3)) break;
    }
    cout << string_buffer.str() << endl;

    for (auto& curve : curves) {
        string_buffer.str("");
        string_buffer << curve->Report(values);
        for (int i = 0; i < values.size(); i++) {
            position_shift = positions[i + 1] - string_buffer.str().length();
            if (position_shift>0)
                string_buffer << string(position_shift, ' ');
            string_buffer << values[i];
        }
        values.clear();

        if (!isnan(t)) {
            c3DPoint point = curve->Coordinate(t);
            string_buffer << string(positions[point_column] - string_buffer.str().length(), ' ');
            string_buffer << point.x;
            string_buffer << string(positions[point_column + 1] - string_buffer.str().length(), ' ');
            string_buffer << point.y;
            string_buffer << string(positions[point_column + 2] - string_buffer.str().length(), ' ');
            string_buffer << point.z;

            c3DPoint derivative = curve->Derivative(t);
            string_buffer << string(positions[derivative_column] - string_buffer.str().length(), ' ');
            string_buffer << derivative.x;
            string_buffer << string(positions[derivative_column + 1] - string_buffer.str().length(), ' ');
            string_buffer << derivative.y;
            string_buffer << string(positions[derivative_column + 2] - string_buffer.str().length(), ' ');
            string_buffer << derivative.z;
        }
        cout << string_buffer.str() << endl;
    }
}

// simple time logger for measurements
class cTimeLogger {
public:
    cTimeLogger() {
        previous_time = chrono::high_resolution_clock::now();
    }
    
    inline float Delta() {
        chrono::high_resolution_clock::time_point new_time = chrono::high_resolution_clock::now();
        unsigned long delta = chrono::duration_cast<std::chrono::microseconds> (new_time - previous_time).count();
        previous_time = new_time;
        return float(delta) / 1000;
    }

    chrono::high_resolution_clock::time_point previous_time;
};

int main()
{
    // task 7 - load .dll with curve classes
    string libName = "3DCurveDLL.dll";
    HMODULE CurveDLL = LoadLibraryA(libName.c_str());
    if (!CurveDLL) {
        cerr << "Unable to load the library" << libName << endl;
        return 1;
    }
    // task 7 - import functions that create curves
    СreateHorizontalCircle = (curve_dll_circle_type)GetProcAddress(CurveDLL, "CreateHorizontalCircle");
    СreateHorizontalEllipse = (curve_dll_ellipse_type)GetProcAddress(CurveDLL, "CreateHorizontalEllipse");
    СreateVertical3DHelix = (curve_dll_helix_type)GetProcAddress(CurveDLL, "CreateVertical3DHelix");
    
    // task 1   - make classes, done elsewhere
    // task 2   - making 12 random curves for demonstration
    cout << string(25, '*') << " Example with 12 random curves " << string(25, '*') << endl;
    vector< shared_ptr<cParametrized3DCurve> > vector1 = make_curves(12);

    // task 3   - showing the data with coordinates and derivatives at t=PI/4
    print_curves<cParametrized3DCurve>(vector1, M_PI_4);

    // task 4   - making another vector with all circles from the 1st
    cout << endl << string(25, '*') << " Example with 12 random curves - only circles " << string(25, '*') << endl;
    vector< shared_ptr<cHorizontalCircle> > vector2;
    for (auto& curve : vector1) {
        shared_ptr<cHorizontalCircle> hCircle = dynamic_pointer_cast<cHorizontalCircle>(curve);
        if (hCircle)
            vector2.push_back(hCircle);
    }
    print_curves<cHorizontalCircle>(vector2);

    // task 5   - sorting the 2nd vector
    cout << endl << string(25, '*') << " Example with 12 random curves - sorted circles " << string(25, '*') << endl;
    sort(vector2.begin(), vector2.end(), [](const std::shared_ptr<cHorizontalCircle>& circle1,
        const std::shared_ptr<cHorizontalCircle>& circle2) {
            return circle1->radius < circle2->radius;
        });
    print_curves<cHorizontalCircle>(vector2);

    // task 6   - computing the total sum of radii
    cout << endl;
    float sum_radii = transform_reduce(vector2.begin(), vector2.end(), 0.0, plus<>(),
        [](const std::shared_ptr<cHorizontalCircle>& circle) { return circle->radius; });
    cout << "Sum of radii in the second vector: " << sum_radii << endl << endl;

    // measuring execution time of different tasks for many curves
    int chrono_test_size = 1000000;
    cout << string(25, '*') << " Time measurement with " << chrono_test_size << " curves " << string(25, '*') << endl;

    // creating curves
    cTimeLogger TimeLogger;
    vector< shared_ptr<cParametrized3DCurve> > vector1_large = make_curves(chrono_test_size);
    cout << "Fill vector 1 duration: " << TimeLogger.Delta() << " ms" << endl;

    // selecting circles
    vector< shared_ptr<cHorizontalCircle> > vector2_large;
    for (auto& curve : vector1_large) {
        shared_ptr<cHorizontalCircle> hCircle = dynamic_pointer_cast<cHorizontalCircle>(curve);
        if (hCircle)
            vector2_large.push_back(hCircle);
    }
    cout << "Fill vector 2 duration: " << TimeLogger.Delta() << " ms" << endl;

    // sorting circles
    sort(vector2_large.begin(), vector2_large.end(), [](const std::shared_ptr<cHorizontalCircle>& circle1,
        const std::shared_ptr<cHorizontalCircle>& circle2) {
            return circle1->radius < circle2->radius;
        }
    );
    cout << "Sort vector 2 duration: " << TimeLogger.Delta() << " ms" << endl;

    // sum with for
    float sum_radii_for = 0;
    for (auto& curve : vector2_large) {
        sum_radii_for += curve->radius;
    }
    cout << "Calculating sum with for: " << TimeLogger.Delta() << " ms" << endl;

    // sum with transform_reduce
    sum_radii = transform_reduce(vector2_large.begin(), vector2_large.end(), 0.0, plus<>(),
        [](const std::shared_ptr<cHorizontalCircle>& circle) { return circle->radius; });
    cout << "Calculating sum with transform_reduce: " << TimeLogger.Delta() << " ms" << endl;

    // task 8 - sum with OpenMP parallel for
    float sum_radii_omp = 0;
    omp_set_num_threads(16);
#pragma omp parallel for reduction(+:sum_radii_omp)
    for (int i = 0; i < vector2_large.size(); i++) {
        sum_radii_omp += vector2_large[i]->radius;
    }
    cout << "Calculating sum with OpenMP for: " << TimeLogger.Delta() << " ms" << endl;

    // sum with parallel transform_reduce
    float sum_radii_parallel = transform_reduce(execution::par, vector2_large.begin(), vector2_large.end(), 0.0, plus<>(),
        [](const std::shared_ptr<cHorizontalCircle>& circle) { return circle->radius; });
    cout << "Calculating sum with parallel transform_reduce: " << TimeLogger.Delta() << " ms" << endl;

    // check that all methods give the same sum
    cout << scientific << "Those sums should be equial: "
        << sum_radii_for << " "
        << sum_radii_omp << " "
        << sum_radii << " "
        << sum_radii_parallel << endl;

    FreeLibrary(CurveDLL);
    _getch();
}

