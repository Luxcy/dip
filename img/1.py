from PIL import Image
from PIL import ImageDraw
# im=Image.open('20.png')

# im.save('20.bmp')

im_board = Image.new("RGB", (500, 500), (255, 255, 255))

drawobject = ImageDraw.Draw(im_board)
drawobject.ellipse((100,150,300,450),fill = (0,0,0))
drawobject.ellipse((50,80,60,90),fill = (0,0,0))
im_board.save('222.bmp')
