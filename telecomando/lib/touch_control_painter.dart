import 'package:flutter/material.dart';

class TouchControlPainter extends CustomPainter {
  double userY, userX, maxRadiusScale;

  TouchControlPainter(this.userX, this.userY, this.maxRadiusScale);

  @override
  void paint(Canvas canvas, Size size) {
    final joystickPaint = new Paint()
      ..color = Colors.blue[400]
      ..style = PaintingStyle.fill;

    final maxRadiusPaint = new Paint()
      ..color = Colors.black
      ..style = PaintingStyle.stroke;

    double joystickRadius = (size.width < size.height ? size.width : size.height)*(1/18);
    double maxRadius = (size.width < size.height ? size.width : size.height) * maxRadiusScale;

    print('position: ');
    print(userX);
    print(userY);

    if(userX <= 0.0 || userY <= 0.0) {
      userX = size.width/2;
      userY = size.height/2;
    }

    canvas.drawCircle(new Offset(size.width/2, size.height/2), maxRadius, maxRadiusPaint);

    canvas.drawCircle(new Offset(userX, userY), joystickRadius, joystickPaint);
  }

  @override
  bool shouldRepaint(TouchControlPainter old) => userX != old.userX && userY != old.userY;
}