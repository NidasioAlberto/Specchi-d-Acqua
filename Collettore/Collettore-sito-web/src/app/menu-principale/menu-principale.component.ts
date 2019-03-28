import { Component, OnInit } from '@angular/core';
import { DatabaseService } from '../core/database.service';
import { Router, NavigationStart, ActivatedRoute } from '@angular/router';
import { MatBottomSheet } from '@angular/material/bottom-sheet';
import { ImpostazioniGraficiComponent } from './impostazioni-grafici/impostazioni-grafici.component';
import { ImpostazioniGrafici } from '../core/tipi';

@Component({
  selector: 'app-menu-principale',
  templateUrl: './menu-principale.component.html',
  styleUrls: ['./menu-principale.component.scss']
})
export class MenuPrincipaleComponent implements OnInit {

  paginaCorrente: string = '/mappa'

  constructor(private database: DatabaseService, private router: Router, private bottomSheet: MatBottomSheet, private activatedRoute: ActivatedRoute) { }

  ngOnInit() {
    this.router.events.subscribe(event => {
      if(event instanceof NavigationStart) {
        this.paginaCorrente = event.url.split('?')[0]
      }
    })
  }

  apriImpostazioni() {
    if(this.paginaCorrente == '/grafici') this.bottomSheet.open(ImpostazioniGraficiComponent, { data: this.activatedRoute.snapshot.queryParams }).afterDismissed().subscribe((impostazioni: ImpostazioniGrafici) => {
      console.log(impostazioni)

      this.router.navigate([], { relativeTo: this.activatedRoute, queryParams: impostazioni, queryParamsHandling: 'merge' })
    })
  }
}
