import 'package:flutter/material.dart';
import 'dart:core';
import 'dart:math';
import 'dart:io';
import 'package:shared_preferences/shared_preferences.dart';

import './touch_control_painter.dart';

enum ConnectionStatus { disconnected, connecting, connected }

class TrolleyController extends StatefulWidget {

  final double x, y;
  final ValueChanged<Offset> onChanged;

  const TrolleyController({
    Key key,
    this.onChanged,
    this.x:0.0,
    this.y:0.0,
  }) : super(key: key);

  @override
  TrolleyControllerState createState() => new TrolleyControllerState();
}

class TrolleyControllerState extends State<TrolleyController> {
  ConnectionStatus connectionStatus = ConnectionStatus.disconnected;

  static final double maxRadiusScale = 3/9;

  double userX = 0.0;
  double userY = 0.0;

  double speed = 0.0; //from 0% to 100%
  double direction = 0.0; //angle in radiants from -Pi to Pi 

  String _ipAddress;
  int _port;

  final _settingsFormKey = GlobalKey<FormState>();

  Socket _connection;

  TrolleyControllerState() {
    _getConnectionData();
  }

  @override
  Widget build(BuildContext context) {
    return new Stack(
      children: <Widget>[
        ConstrainedBox(
          constraints: new BoxConstraints.expand(),
          child: new GestureDetector(
            behavior: HitTestBehavior.opaque,
            onPanStart:_handlePanStart,
            onPanEnd: _handlePanEnd,
            onPanUpdate: _handlePanUpdate,
            child: new CustomPaint(
              size: new Size(userX, userY),
              painter: new TouchControlPainter(userX, userY, maxRadiusScale),
            ),
          ),
        ),
        new Column(
          mainAxisAlignment: MainAxisAlignment.end,
          children: <Widget>[
            new ButtonBar(
              children: <Widget>[
                new IconButton(
                  icon: Icon(Icons.block),
                  tooltip: 'Premi per fermare la barchetta',
                  color: Colors.redAccent,
                  onPressed: () => _fermaBarchetta(),
                ),
                new Stack(
                  alignment: Alignment.center,
                  children: <Widget>[
                    connectionStatus == ConnectionStatus.connecting ? CircularProgressIndicator(
                      valueColor: AlwaysStoppedAnimation<Color>(Colors.amberAccent),
                    ) : null,
                    new IconButton(
                      icon: connectionStatus == ConnectionStatus.disconnected ? Icon(Icons.link_off) : Icon(Icons.link),
                      tooltip: connectionStatus == ConnectionStatus.connected ? 'tap to disconnect' : 'tap to connect',
                      color: getLinkColor(),
                      onPressed: () {
                        print('link button pressed');
                        setState(() {
                          if(connectionStatus == ConnectionStatus.disconnected) connect();
                          else disconnect();
                        });
                      },
                    ),
                  ].where((w) => w != null).toList(),
                ),
                new IconButton(
                  icon: Icon(Icons.settings),
                  tooltip: 'tap to enter settings',
                  color: Colors.cyan,
                  onPressed: () {
                    print('settings button pressed');

                    //show the settings dialog
                    openSettingsDialog();
                  },
                ),
              ],
            )
          ],
        )
      ]
    );
  }

  Color getLinkColor() {
    if(connectionStatus == ConnectionStatus.connected) return Colors.greenAccent;
    if(connectionStatus == ConnectionStatus.connecting) return Colors.amberAccent;
    else return Colors.redAccent;
  }

  void disconnect() {
      setState(() {
        connectionStatus = ConnectionStatus.disconnected;
      });

    if(_connection != null) _connection.close().then((_) {
      _connection = null;
      print('connection closed');
    });
  }

  void connect() {
    if(_ipAddress != null && _port != null) {
      setState(() {
        connectionStatus = ConnectionStatus.connecting;
      });
      print('trying to connect');

      Socket.connect(_ipAddress, _port, timeout: Duration(seconds: 10)).then((socket) {
        print('connected');
        _connection = socket;

        setState(() {
          connectionStatus = ConnectionStatus.connected;
        });

        _connection.done.then((_) {
          _connection = null;
          print('connection closed');
        });
      }).catchError((error) {
        print(error);

        _connection = null;

        setState(() {
          connectionStatus = ConnectionStatus.disconnected;
        });
      });
    } else {
      showDialog(
        context: context,
        builder: (BuildContext context) {
          return AlertDialog(
            title: Text('Value missing !'),
            content: Text('Enter a valid ip address and port !'),
            actions: <Widget>[
              new FlatButton(
                child: Text('OK'),
                onPressed: () => Navigator.pop(context),
              ),
              new FlatButton(
                child: Text('SETTINGS'),
                onPressed: () {
                  Navigator.pop(context);
                  openSettingsDialog();
                },
              )
            ],
          );
        }
      );
    }
  }

  void reconnect() {
    disconnect();
    connect();
  }

  void openSettingsDialog() {
    showDialog(
      context: context,
      builder: (BuildContext context) {
        return AlertDialog(
          title: Text('Settings'),
          content: new Form(
            key: _settingsFormKey,
            child: Column(
              mainAxisSize: MainAxisSize.min,
              children: <Widget>[
                TextFormField(
                  decoration: InputDecoration(
                    labelText: 'Ip address',
                    hintText: '192.168.0.2'
                  ),
                  validator: (text) {
                    RegExp ipAddressPattern = new RegExp(r'^\d{1,3}.\d{1,3}.\d{1,3}.\d{1,3}$');
                    if(!ipAddressPattern.hasMatch(text)) return 'Enter a valid ip address';
                  },
                  onSaved: (text) {
                    _ipAddress = text;
                  },
                  initialValue: _ipAddress,
                ),
                TextFormField(
                  decoration: InputDecoration(
                    labelText: 'Port',
                    hintText: '1234'
                  ),
                  validator: (text) {
                    if(text.isEmpty || int.tryParse(text) == null) {
                      return 'Enter a valid port number';
                    } else if(int.parse(text) <= 0 || int.parse(text) >= 65535) {
                      return 'Enter a valid port number';
                    }
                  },
                  onSaved: (text) {
                    _port = int.parse(text);
                  },
                  initialValue: _port.toString(),
                ),
              ]
            )
          ),
          actions: <Widget>[
            new FlatButton(
              child: Text('CANCEL'),
              onPressed: () {
                Navigator.pop(context);
              },
            ),
            new FlatButton(
              child: Text('SAVE'),
              onPressed: () {
                if(_settingsFormKey.currentState.validate()) {
                  _settingsFormKey.currentState.save();
                  Navigator.pop(context);

                  reconnect();

                  _saveConnectionData();
                }
              },
            )
          ],
        );
      }
    );
  }

  void _onUserPositionChanged(Offset offset) {
    final RenderBox referenceBox = context.findRenderObject();
    Offset position = referenceBox.globalToLocal(offset);
    if(widget.onChanged != null) widget.onChanged(position);

    userX = position.dx;
    userY = position.dy;
    
    double maxRadius = (referenceBox.size.width < referenceBox.size.height ? referenceBox.size.width : referenceBox.size.height) * maxRadiusScale;

    //process the user position

    if(userX <= 0.0 || userY <= 0.0) {
      userX = referenceBox.size.width/2;
      userY = referenceBox.size.height/2;
    } else {
      //compute the distance and the angle from the center
      double distance = sqrt(pow((userX - referenceBox.size.width/2).abs(), 2) + pow((userY - referenceBox.size.height/2).abs(), 2));
      direction = atan2((userX - referenceBox.size.width/2), (userY - referenceBox.size.height/2));
      
      if(distance > maxRadius) {
        //limit the distance
        speed = 100.0;

        userX = referenceBox.size.width/2 + sin(direction) * maxRadius;
        userY = referenceBox.size.height/2 + cos(direction) * maxRadius;
      } else {
        speed = distance/maxRadius * 100;
      }

      direction = direction < 0 ? pi + direction : direction - pi;
    }

    setState(() {
      userX = userX;
      userY = userY;
    });

    _sendData(speed, direction);
  }

  void _sendData(num speed, num direction) {
    print('Speed: ' + speed.toStringAsPrecision(2) + ' direction: ' + direction.toStringAsPrecision(2));
    //send the data to the trolley if connected
    if(_connection != null) {
      print('data sent to the device');
      _connection.write(speed.toStringAsPrecision(2) + '@' + direction.toStringAsPrecision(2) + '\n');
    }
  }

  void _fermaBarchetta() {
    _sendData(0, 0);

    setState(() {
      userX = 0;
      userY = 0;
    });
  }
  
  void _handlePanStart(DragStartDetails details) {
    _onUserPositionChanged(details.globalPosition);
  }

  void _handlePanEnd(DragEndDetails details) {
    //_onUserPositionChanged(new Offset(0.0, 0.0));
  }

  void _handlePanUpdate(DragUpdateDetails details) {
    _onUserPositionChanged(details.globalPosition);
  }

  void _saveConnectionData() {
    SharedPreferences.getInstance().then((sharedPreferences) {
      sharedPreferences.setString('ipAddress', _ipAddress);
      sharedPreferences.setInt('port', _port);
    });
  }

  void _getConnectionData() {
    SharedPreferences.getInstance().then((sharedPreferences) {
      _ipAddress = sharedPreferences.getString('ipAddress');
      _port = sharedPreferences.getInt('port');
    });
  }
}