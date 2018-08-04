# Simple algorithm project for university
# By NimaH79
# NimaH79.ir

from operator import itemgetter

rectangles = []
while 1:
    rectangle = input()
    if rectangle == 'finish':
        break
    rectangle = rectangle.split(',')
    if len(rectangle) % 2 == 0:
        while len(rectangle) >= 3:
            a = int(rectangle[0])
            b = int(rectangle[1])
            c = int(rectangle[2])
            while a < c:
                rectangles.append([a, b, a + 1])
                a += 1
            rectangle.pop(0)
            rectangle.pop(0)
            rectangle.pop(0)
    else:
        while len(rectangle) >= 3:
            a = int(rectangle[0])
            b = int(rectangle[1])
            c = int(rectangle[2])
            while a < c:
                rectangles.append([a, b, a + 1])
                a += 1
            rectangle.pop(0)
            rectangle.pop(0)
rectangles = sorted(rectangles, key=itemgetter(0, 2, 1))
result = []
previous_height = 0
for i in range(rectangles[0][0], rectangles[len(rectangles) - 1][0] + 1):
    max_height = 0
    for j in range(len(rectangles)):
        if rectangles[j][0] == i and rectangles[j][1] > max_height:
            max_height = rectangles[j][1]
    if max_height != previous_height:
        result.append(i)
        result.append(max_height)
    previous_height = max_height
result.append(rectangles[len(rectangles) - 1][2])
result = ','.join(map(str, result))
print(result)
