import { map } from 'rxjs/operators'
import { Observable } from 'rxjs'
import { AngularFirestore, DocumentReference } from '@angular/fire/firestore'
import { Injectable } from '@angular/core'
import { Record } from './types/record'
import { Sensore } from './types/sensore'

@Injectable({
  providedIn: 'root'
})
export class FirestoreService {

  constructor(public firestore: AngularFirestore) { }

  ottieniRecord() {
    console.log('ottieniRecord')
    return this.firestore.collection<Record>('Record').valueChanges()
  }

  ottieniRecordDivisi(record?: Observable<Record[]>) {
    if(record == undefined) record = this.ottieniRecord()
    return record.pipe(
      map(record => {
        //recupero tutte le diverse barchette
        let barche = []
        record.forEach(singolo => {
          if(!barche.includes(singolo.record.idBarchetta.path)) barche.push(singolo.record.idBarchetta.path)
        })

        //suddivido i record x barchette
        let record2: Record[][] = new Array(barche.length)
        record.forEach(singolo => {
          let index = barche.indexOf(singolo.record.idBarchetta.path)

          if(record2[index] == undefined) record2[index] = []

          record2[index].push(singolo)
        })
        return record2
      })
    )
  }

  ottieniSensore(sensore: DocumentReference) {
    return this.firestore.doc<Sensore>(sensore).valueChanges()
  }
}
