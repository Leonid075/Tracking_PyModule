import spam
import cv2
a=cv2.imread("C:\\Users\\Grebe\\Desktop\\1.jpg")
a=cv2.resize(a, (int(416),int(416)))
spam.system(a)
print(1)
