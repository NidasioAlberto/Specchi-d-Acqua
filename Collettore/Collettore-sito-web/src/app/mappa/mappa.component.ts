import { Component, OnInit } from '@angular/core'
import { DatabaseService } from '../core/database.service'
import { Observable } from 'rxjs'
import { map } from 'rxjs/operators'
import { Barchetta, Record } from '../core/tipi'

@Component({
  selector: 'app-mappa',
  templateUrl: './mappa.component.html',
  styleUrls: ['./mappa.component.scss']
})
export class MappaComponent implements OnInit {

  recordDivisi: Observable<{
    record: Record[]
    barchetta: Barchetta
    colore?: string
  }[]>

  constructor(public database: DatabaseService) { }

  ngOnInit() {
    this.recordDivisi = this.database.ottieniRecordDivisiPerBarchette(600, true).pipe(
      map(record => {
        record.map(element => {
          element['colore'] = '#'+(Math.random()*0xFFFFFF<<0).toString(16)
          return element
        })
        return record
      })
    )
  }
}