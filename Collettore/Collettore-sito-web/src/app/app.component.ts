import { Record } from './core/types/record'
import { Observable } from 'rxjs'
import { Component, OnInit } from '@angular/core'
import { RouterOutlet, Router } from '@angular/router'
import { slider } from './route-animations'
import { DatabaseService } from './core/database.service'

@Component({
  selector: 'app-root',
  animations: [
    slider
  ],
  templateUrl: './app.component.html',
  styleUrls: ['./app.component.scss'],
})
export class AppComponent implements OnInit {

  record: Observable<Record[]>

  constructor(public database: DatabaseService) { }

  ngOnInit() {
    this.record = this.database.ottieniRecord()
  }

  prepareRoute(outlet: RouterOutlet) {
    return outlet && outlet.activatedRouteData && outlet.activatedRouteData['animation'];
  }
}
