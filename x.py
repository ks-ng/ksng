from math import sqrt, asin, degrees
# AM = MC = sqrt(AB^2 + BC^2)/2
# p/h = mc/bc = sin theta -> sin inverse (p/h) = theta

while True:
    try:
        ab = int(input())
        bc = int(input())
    except EOFError:
        break
    
    ac = sqrt((ab ** 2) + (bc ** 2))
    mc = ac/2
    
    angle = str(int(round(degrees(asin(mc/bc)), 0)))
    
    print(angle + chr(176))
