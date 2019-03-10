import { map } from 'rxjs/operators'
import { Observable, combineLatest } from 'rxjs'
import { AngularFirestore, DocumentReference } from '@angular/fire/firestore'
import { AngularFireDatabase } from '@angular/fire/database'
import { Injectable } from '@angular/core'
import { Record } from './types/record'
import { Sensore } from './types/sensore'
import { Barchetta } from './types/barchetta';
import { DatiSensore } from './types/dati-sensore';

@Injectable({
  providedIn: 'root'
})
export class DatabaseService {
  constructor(public firestore: AngularFirestore, public fDatabase: AngularFireDatabase) { }

  ottieniRecord() {
    console.log('ottieniRecord')

    return this.fDatabase.list<Record>('Record', ref => ref.orderByChild('dateTime').limitToLast(16)).valueChanges().pipe(
      map(record => {
        record.forEach(element => {
          element.idBarchetta = this.firestore.collection('Barchette').doc(String(element.idBarchetta)).ref

          element.datiSensori.forEach(element2 => {
            element2.idSensore = this.firestore.collection('Sensori').doc(String(element2.idSensore)).ref
          })
        })

        return record.reverse()
      })
    )

    //return this.firestore.collection<Record>('Record', ref => ref.orderBy('record.dateTime', 'desc').limit(5)).valueChanges()
  }

  ottieniRecordDivisi(record?: Observable<Record[]>) {
    console.log('ottieniRecordDivisi')
    if(record == undefined) record = this.ottieniRecord()
    return record.pipe(
      map(record => {
        //recupero tutte le diverse barchette
        let barche = []
        record.forEach(singolo => {
          if(!barche.includes(singolo.idBarchetta.path)) barche.push(singolo.idBarchetta.path)
        })

        //suddivido i record x barchette
        let record2: Record[][] = new Array(barche.length)
        record.forEach(singolo => {
          let index = barche.indexOf(singolo.idBarchetta.path)

          if(record2[index] == undefined) record2[index] = []

          record2[index].push(singolo)
        })
        return record2
      })
    )
  }

  ottieniSensore(sensore: DocumentReference) {
    console.log('ottieniSensore')
    return this.firestore.doc<Sensore>(sensore).valueChanges()
  }

  ottieniBarchette() {
    console.log('ottieniBarchette')
    return this.firestore.collection<Barchetta>('Barchette').snapshotChanges().pipe(
      map(actions => actions.map(action => {
        let barchetta = action.payload.doc.data() as Barchetta
        barchetta.id = action.payload.doc.ref.id
        return barchetta
      }))
    )

    //return this.firestore.collection<Barchetta>('Barchette').valueChanges()
  }

  ottieniBarchetta(barchetta: DocumentReference) {
    console.log('ottieniBarchetta')
    return this.firestore.doc<Barchetta>(barchetta).valueChanges()
  }

  ottieniUltimaPosizioneBarchetta(idBarchetta: string) {
    console.log('ottieniUltimaPosizioneBarchetta')
    return this.fDatabase.list<Record>('Record', ref => ref.orderByChild('idBarchetta').equalTo(idBarchetta).limitToLast(1)).valueChanges().pipe(
      map(valori => {
        let valore = valori.pop()
        if(valore != undefined) {
          let dato = {
            latitudine: valore.latitudine,
            longitudine: valore.longitudine,
            dateTime: valore.dateTime
          }
          return dato
        } else return undefined
      })
    )
  }

  ottieniSensoriRecord(datiSensori: DatiSensore[]) {
    console.log('ottieniSensoriRecord')
    datiSensori.forEach(datiSensore => {
      datiSensore.sensore = this.ottieniSensore(datiSensore.idSensore)
    })

    return datiSensori
  }

  ottieniSensori() {
    console.log('ottieniSensori')
    return this.firestore.collection<Sensore>('Sensori').valueChanges()
  }
}
