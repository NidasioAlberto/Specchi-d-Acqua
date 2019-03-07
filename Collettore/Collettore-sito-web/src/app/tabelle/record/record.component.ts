import {Component, OnInit, ViewChild} from '@angular/core'
import { FirestoreService } from 'src/app/core/firestore.service'
import { Observable } from 'rxjs'
import { Record } from 'src/app/core/types/record'
import { animate, state, style, transition, trigger } from '@angular/animations'
import { DatiSensore } from 'src/app/core/types/dati-sensore'
import { MatDialog } from '@angular/material';
import { DialogDatiSensoreComponent } from './dialog-dati-sensore/dialog-dati-sensore.component';

@Component({
  selector: 'app-record',
  templateUrl: './record.component.html',
  styleUrls: ['./record.component.scss'],
  animations: [
    trigger('detailExpand', [
      state('collapsed', style({height: '0px', minHeight: '0', display: 'none'})),
      state('expanded', style({height: '*'})),
      transition('expanded <=> collapsed', animate('225ms cubic-bezier(0.4, 0.0, 0.2, 1)')),
    ]),
  ],
})
export class RecordComponent implements OnInit {

  record: Observable<Record[]>

  displayedColumns: string[] = ['id', 'latitudine', 'longitudine', 'altitudine', 'dateTime', 'satelliti', 'velocita']

  constructor(public firestore: FirestoreService, public dialog: MatDialog) { }

  ngOnInit() {
    this.record = this.firestore.ottieniRecord()
  }

  mostraDatiSensore(datiSensore: DatiSensore) {
    this.dialog.open(DialogDatiSensoreComponent, { data: datiSensore }).afterClosed().subscribe(result => {
      console.log('The dialog was closed', result);
    });
  }
}