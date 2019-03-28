import { Component, OnInit } from '@angular/core'
import { Observable } from 'rxjs'
import { DatabaseService } from 'src/app/core/database.service'
import { Sensore } from 'src/app/core/tipi';

@Component({
  selector: 'app-sensori',
  templateUrl: './sensori.component.html',
  styleUrls: ['./sensori.component.scss']
})
export class SensoriComponent implements OnInit {

  displayedColumns: string[] = ['nome', 'note', 'unita', 'modifica']

  sensori: Observable<Sensore[]>

  constructor(public database: DatabaseService) { }

  ngOnInit() {
    this.sensori = this.database.ottieniSensori()
  }

  modificaSensore(sensore: Sensore) {
    console.log(sensore)
  }
}
