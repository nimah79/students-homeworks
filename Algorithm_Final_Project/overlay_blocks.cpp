/*
Simple algorithm project for university
By NimaH79
NimaH79.ir
**/

#include <bits/stdc++.h>
using namespace std;
 
struct Rectangle 
{
    int x_start;
    int height;
    int x_end;
}; 

bool compareTwoRectangles(Rectangle a, Rectangle b)
{
    if(a.x_start != b.x_start)
        return a.x_start < b.x_start;

    if(a.x_end != b.x_end)
        return a.x_end < b.x_end;
 
    return a.height < b.height;
}

void sortRectangles(Rectangle a[], int n)
{
    sort(a, a + n, compareTwoRectangles);
}

int main()
{
    int n, result[1000], result_count = 0, previous_height = 0, max_height;

    cin >> n;

    Rectangle rectangles[n];

    for(int i = 0; i < n; i++) {
        cin >> rectangles[i].x_start >> rectangles[i].height >> rectangles[i].x_end;
    }
 
    sortRectangles(rectangles, n);

    for(int i = rectangles[0].x_start; i < rectangles[n - 1].x_end; i++)
    {
        max_height = 0;
        for(int j = 0; j < n; j++)
        {
            if(rectangles[j].x_start <= i && rectangles[j].x_end > i && rectangles[j].height > max_height)
                max_height = rectangles[j].height;
        }
        if(max_height != previous_height) {
            result[result_count] = i;
            result_count++;
            result[result_count] = max_height;
            result_count++;
        }
        previous_height = max_height;
    }

    result[result_count] = rectangles[n - 1].x_end;
    result_count++;

    for(int i = 0; i < result_count; i++) {
        if (i > 0)
            cout << ",";
        cout << result[i];
    }
 
    return 0;
}
