import { Record } from './core/types/record';
import { Observable } from 'rxjs';
import { Component, OnInit } from '@angular/core'
import { FirestoreService } from './core/firestore.service'
import { RouterOutlet, Router } from '@angular/router'
import { slider } from './route-animations'

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

  constructor(public firestore: FirestoreService) { }

  ngOnInit() {
    this.record = this.firestore.ottieniRecord()
  }

  prepareRoute(outlet: RouterOutlet) {
    return outlet && outlet.activatedRouteData && outlet.activatedRouteData['animation'];
  }
}
