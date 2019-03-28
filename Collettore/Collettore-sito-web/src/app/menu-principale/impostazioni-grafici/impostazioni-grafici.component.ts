import { Component, OnInit, Inject } from '@angular/core';
import { MatBottomSheetRef, MAT_BOTTOM_SHEET_DATA } from '@angular/material/bottom-sheet';
import { Barchetta, Sensore, ImpostazioniGrafici } from 'src/app/core/tipi';
import { Observable } from 'rxjs';
import { DatabaseService } from 'src/app/core/database.service';

@Component({
  selector: 'app-impostazioni-grafici',
  templateUrl: './impostazioni-grafici.component.html',
  styleUrls: ['./impostazioni-grafici.component.scss']
})
export class ImpostazioniGraficiComponent implements OnInit {

  modalita: string = 'sensori'

  barchette: Barchetta[]
  barchetta: string = '1'

  sensori: Sensore[]
  sensore: string = '1'

  constructor(private bottomSheetRef: MatBottomSheetRef<ImpostazioniGraficiComponent>, @Inject(MAT_BOTTOM_SHEET_DATA) public data: ImpostazioniGrafici, private database: DatabaseService) { }

  ngOnInit() {
    console.log(this.data)
    if(this.data != undefined && this.data.modalita == 'barchette') {
      this.modalita = 'barchette'
      this.barchetta = this.data.oggetto
    }
    this.database.ottieniBarchette().subscribe(array => this.barchette = array)

    if(this.data != undefined && this.data.modalita == 'sensori') {
      this.modalita = 'sensori'
      this.sensore = this.data.oggetto
    }
    this.database.ottieniSensori().subscribe(array => this.sensori = array)
  }

  salvaImpostazioni() {
    this.bottomSheetRef.dismiss({
      modalita: this.modalita,
      oggetto: (this.modalita == 'barchette' ? this.barchetta : this.sensore)
    })
  }
}
