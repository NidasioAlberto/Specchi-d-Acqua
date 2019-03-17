import 'package:flutter/material.dart';
import './utils/stato_connessione.dart';
import 'dart:io';
import 'package:shared_preferences/shared_preferences.dart';
import './touch_control_painter.dart';
import 'dart:math';
import 'package:vector_math/vector_math.dart' as vector;

class Telecomando extends StatefulWidget {

  const Telecomando({Key key}) : super(key: key);

  @override
  TelecomandoState createState() => new TelecomandoState();
}

class TelecomandoState extends State<Telecomando> {
  ///stato della connessione
  StatoConnessione _statoConnessione = StatoConnessione.disconnesso;

  ///indirizzo ip della barchetta
  String _indirizzoIp;

  ///porta della barchetta
  int _porta;

  ///chiave per accedere alle impostazioni
  final _settingsFormKey = GlobalKey<FormState>();

  ///socket che ci collega con la barchetta
  Socket _socket;

  ///posizione dell'utente su x
  double userX = 0.0;
  ///posizione dell'utente su y
  double userY = 0.0;

  //dimensione del joystick
  static final double maxRadiusScale = 3/9;

  ///velocità della barchetta da 0% a 100%
  double velocita = 0.0;

  //direzione della barchetta (avanti o indietro) 1 per avanti, -1 per indietro
  int direzione = 1;

  //angolo di direzione (da destra a sinistra) in gradi da -90 a 90, lo 0 punta avanti
  int angolo = 0;

  TelecomandoState() {
    _recuperaImpostazioni();
  }

  @override
  Widget build(BuildContext context) {
    return new Stack(
      children: <Widget>[
        //background principale che comprende il joystick
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
        //lista dei comandi in basso a destra
        new Column(
          mainAxisAlignment: MainAxisAlignment.end,
          children: <Widget>[
            new ButtonBar(
              children: <Widget>[
                //bottone per fermare la barchetta
                new IconButton(
                  icon: Icon(Icons.block),
                  tooltip: 'Premi per fermare la barchetta',
                  color: Colors.redAccent,
                  onPressed: () {
                    print('Bottone stop premuto');
                    _inviaDati(0, 0, 0);
                  },
                ),
                //bottone e indicatore per la connessione
                new Stack(
                  alignment: Alignment.center,
                  children: <Widget>[
                    _statoConnessione == StatoConnessione.inConnessione ? CircularProgressIndicator(
                      valueColor: AlwaysStoppedAnimation<Color>(Colors.amberAccent),
                    ) : null,
                    new IconButton(
                      icon: _statoConnessione == StatoConnessione.disconnesso ? Icon(Icons.link_off) : Icon(Icons.link),
                      tooltip: _statoConnessione == StatoConnessione.connesso ? 'Premi per scollegarti' : 'Premi per collegarti',
                      color: _ottieniColoreLink(),
                      onPressed: () {
                        print('Bottone connessione premuto');

                        //provo a connettermi o disconnettemri a seconda del caso
                        setState(() => _statoConnessione == StatoConnessione.disconnesso ? _connetti() : _disconnetti());
                      },
                    ),
                  ].where((w) => w != null).toList(), //eliminiamo gli oggetti nulli dalla lista, in questo modo se lo stato della connessione non è 'inConnessione' lamprogress bar non verrà mostrata 😀
                ),
                //bottone impostazioni
                new IconButton(
                  icon: Icon(Icons.settings),
                  tooltip: 'tap to enter settings',
                  color: Colors.cyan,
                  onPressed: () {
                    print('Bottone impostazioni premuto');

                    //apro il dialog impostazioni
                    _mostraDialogImpostazioni();
                  },
                ),
              ],
            )
          ],
        )
      ]
    );
  }

  /// Restituisce il colore da date al bottone rispetto allo stato attuale della connessione
  _ottieniColoreLink() {
    if(_statoConnessione == StatoConnessione.connesso) return Colors.greenAccent;
    if(_statoConnessione == StatoConnessione.inConnessione) return Colors.amberAccent;
    else return Colors.redAccent;
  }

  ///mostra un avviso di dati anacanti
  _mostraAvvisoDatiMancanti() {
    showDialog(
      context: context,
      builder: (BuildContext context) {
        return AlertDialog(
          title: Text('Impostazioni mancanti'),
          content: Text('Inserisci le impostazioni, subito!'),
          actions: <Widget>[
            new FlatButton(
              child: Text('Ok'),
              onPressed: () => Navigator.pop(context),
            ),
            new FlatButton(
              child: Text('Impostazioni'),
              onPressed: () {
                Navigator.pop(context);
                _mostraDialogImpostazioni();
              },
            )
          ],
        );
      }
    );
  }

  ///mosta il dialog per le impostazioni
  _mostraDialogImpostazioni() {
    showDialog(
      context: context,
      builder: (BuildContext context) {
        return AlertDialog(
          title: Text('Impostazioni'),
          content: new Form(
            key: _settingsFormKey,
            child: Column(
              mainAxisSize: MainAxisSize.min,
              children: <Widget>[
                //indirizzo ip
                TextFormField(
                  decoration: InputDecoration(
                    labelText: 'Indirizzo ip',
                    hintText: '192.168.0.2'
                  ),
                  validator: (text) {
                    RegExp ipAddressPattern = new RegExp(r'^\d{1,3}.\d{1,3}.\d{1,3}.\d{1,3}$');
                    if(!ipAddressPattern.hasMatch(text)) return 'Inserisci un ip valido';
                  },
                  onSaved: (text) {
                    _indirizzoIp = text;
                  },
                  initialValue: _indirizzoIp,
                ),
                //porta
                TextFormField(
                  decoration: InputDecoration(
                    labelText: 'Port',
                    hintText: '1234'
                  ),
                  validator: (text) {
                    if(text.isEmpty || int.tryParse(text) == null) {
                      return 'Inserisci un numero valido';
                    } else if(int.parse(text) <= 0 || int.parse(text) >= 65535) {
                      return 'int.parse(text) <= 0 || int.parse(text) >= 65535';
                    }
                  },
                  onSaved: (text) {
                    _porta = int.parse(text);
                  },
                  initialValue: _porta.toString(),
                ),
              ]
            )
          ),
          actions: <Widget>[
            new FlatButton(
              child: Text('Annulla'),
              onPressed: () {
                Navigator.pop(context);
              },
            ),
            new FlatButton(
              child: Text('Salva'),
              onPressed: () {
                if(_settingsFormKey.currentState.validate()) {
                  _settingsFormKey.currentState.save();
                  Navigator.pop(context);

                  //salvo i dati nella memoria permanente
                  _salvaImpostazioni();

                  //tento di ricollegarmi
                  _riconnessione();
                }
              },
            )
          ],
        );
      }
    );
  }

  ///salvo i dati nella memoria permanente
  _salvaImpostazioni() {
    SharedPreferences.getInstance().then((sharedPreferences) {
      sharedPreferences.setString('indirizzoIp', _indirizzoIp);
      sharedPreferences.setInt('porta', _porta);
    });
  }

  ///recupero le impostazioni dalla memoria permanente
  _recuperaImpostazioni() {
    SharedPreferences.getInstance().then((sharedPreferences) {
      _indirizzoIp = sharedPreferences.getString('indirizzoIp');
      _porta = sharedPreferences.getInt('porta');
    });
  }

  ///tento di collegarmi alla barchetta
  _connetti() {
    //controllo la validità dei dati
    if(_indirizzoIp != null && _porta != null) {
      //imposto lo stato della connessione a 'inConnessione'
      setState(() => _statoConnessione = StatoConnessione.inConnessione);

      print('provo a connettermi');

      //provo a connettermi
      Socket.connect(_indirizzoIp, _porta, timeout: Duration(seconds: 10)).then((socket) {
        print('connesso');

        //salvo il socket
        _socket = socket;

        //imposto lo stato della connessione a 'connesso'
        setState(() => _statoConnessione = StatoConnessione.connesso);
      }).catchError((error) {
        //nel caso di errore rispristino lo stato della connessione a 'disconnesso'
        setState(() => _statoConnessione = StatoConnessione.disconnesso);

        //e imposto il socket a null
        _socket = null;
      });
    } else {
      _mostraAvvisoDatiMancanti();
    }
  }

  ///concludo la connessione con la barchetta
  _disconnetti() {
    //imposto lo stato della connessione
    setState(() => _statoConnessione = StatoConnessione.disconnesso);

    //chiudo il socket
    if(_socket != null) _socket.close().then((_) {
      _socket = null;
      print('socket chiuso');
    });
  }

  //disconnetto e riconnetto l'app alla barchetta
  _riconnessione() {
    _disconnetti();
    _connetti();
  }

  ///gestione inizio spostsmento
  _handlePanStart(DragStartDetails details) {
    _aggiornaPosizioneUtente(details.globalPosition);
  }

  ///gestione fine spostamento
  _handlePanEnd(DragEndDetails details) {
    _aggiornaPosizioneUtente(Offset(0, 0));
  }

  ///gestione aggiornamento posiozine dello spostamento 
  _handlePanUpdate(DragUpdateDetails details) {
    _aggiornaPosizioneUtente(details.globalPosition);
  }

  ///aggiorna la posizione dell'utente disegnando il joystick e inviando i dati di controllo alla barchetta se presente
  _aggiornaPosizioneUtente(Offset offset) {
    //calcolo la posizione del joystick sullo schermo
    final RenderBox referenceBox = context.findRenderObject();
    Offset position = referenceBox.globalToLocal(offset);

    userX = position.dx;
    userY = position.dy;

    //calcolo il raggio del joystick
    double raggioMassimo = (referenceBox.size.width < referenceBox.size.height ? referenceBox.size.width : referenceBox.size.height) * maxRadiusScale;

    //calcolo la velocità, direzione e angolo
    if(userX > 0.0 || userY > 0.0) {
      //calcolo la distanza dal centro dello schermo
      double distanza = sqrt(pow((userX - referenceBox.size.width/2).abs(), 2) + pow((userY - referenceBox.size.height/2).abs(), 2));

      //ottengo l'angolo
      double angoloRadianti = atan2((userX - referenceBox.size.width/2), (userY - referenceBox.size.height/2));
      
      if(distanza > raggioMassimo) {
        //limit the distance
        velocita = 100.0;

        userX = referenceBox.size.width/2 + sin(angoloRadianti) * raggioMassimo;
        userY = referenceBox.size.height/2 + cos(angoloRadianti) * raggioMassimo;
      } else {
        velocita = distanza/raggioMassimo * 100;
      }

      angoloRadianti = angoloRadianti < 0 ? pi + angoloRadianti : angoloRadianti - pi;
      angolo = vector.degrees(angoloRadianti).round();

      direzione = 1;
      if(angolo > 90) {
        angolo = 180 - angolo;
        direzione = -1;
      }
      if(angolo < -90) {
        angolo = -180 - angolo;
        direzione = -1;
      }

      print('Angolo: ' + angolo.toString());
      print('Velocità: ' + velocita.toString());
      print('Direzione: ' + direzione.toString());
    } else {
      userX = referenceBox.size.width/2;
      userY = referenceBox.size.height/2;
    }

    //aggiorno la posizione dell'utente con quella calcolata
    setState(() {
      userX = userX;
      userY = userY;
    });

    _inviaDati(velocita, angolo, direzione);
  }

  ///invio i dati di controllo alla barchetta
  _inviaDati(_velocita, _angolo, _direzione) {
    //controllo se la connessione è disponibile
    if(_statoConnessione ==StatoConnessione.connesso && _socket != null) {
      print('data sent to the device');
      _socket.write(_velocita.floor().toString() + ',' + _angolo.toString() + ',' + _direzione.toString() + ';');
    }
  }
}