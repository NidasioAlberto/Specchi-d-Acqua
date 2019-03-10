import { Component, OnInit } from '@angular/core';
import { DatabaseService } from '../core/database.service';
import { Observable } from 'rxjs';
import { Record } from '../core/types/record';
import { DocumentReference } from '@angular/fire/firestore';
import { map } from 'rxjs/operators';

@Component({
  selector: 'app-mappa',
  templateUrl: './mappa.component.html',
  styleUrls: ['./mappa.component.scss']
})
export class MappaComponent implements OnInit {

  recordDivisi: Observable<{
    record: Record[]
    idBarchetta: DocumentReference
    colore?: string
  }[]>

  constructor(public database: DatabaseService) { }

  ngOnInit() {
    this.recordDivisi = this.database.ottieniRecordDivisi(undefined, 100).pipe(
      map(record => {
        record.map(element => {
          element['colore'] = '#'+(Math.random()*0xFFFFFF<<0).toString(16);
          return element
        })
        return record
      })
    )


    this.recordDivisi.subscribe(console.log)
  }

}
