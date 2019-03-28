import { NgModule } from '@angular/core';
import { Routes, RouterModule } from '@angular/router';
import { MappaComponent } from './mappa/mappa.component';
import { GraficiComponent } from './grafici/grafici.component';
import { TabelleComponent } from './tabelle/tabelle.component';

const routes: Routes = [
  { path: 'mappa', component: MappaComponent },
  { path: 'grafici', component: GraficiComponent },
  { path: 'tabelle', component: TabelleComponent },
  { path: '', redirectTo: '/mappa', pathMatch: 'full'}
];

@NgModule({
  imports: [RouterModule.forRoot(routes)],
  exports: [RouterModule]
})
export class AppRoutingModule { }
