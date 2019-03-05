const mqtt = require('mqtt')
const admin = require('firebase-admin')

var serviceAccount = require('./service-account-key.json')

admin.initializeApp({
    credential: admin.credential.cert(serviceAccount)
})

var firestore = admin.firestore();

var client = mqtt.connect('mqtt://192.168.4.100')

client.on('connect', function () {
    client.subscribe('records', function (err) {
        console.log('Connected')
    })
})

client.on('message', (topic, message) => {
    //decodifico il messaggio
    let messaggio = JSON.parse(message.toString())
    console.log(message.toString())
    
    //aggiusto i dati
    messaggio.datiSensori.forEach(dati => {
        dati.idSensore = firestore.collection('Sensori').doc(dati.idSensore.toString())
    })

    messaggio.record.dateTime = admin.firestore.Timestamp.fromDate(new Date(messaggio.record.dateTime))
    
    //aggiungo i dati al firestore
    firestore.collection('Record').add(messaggio)

    console.log(messaggio)
})