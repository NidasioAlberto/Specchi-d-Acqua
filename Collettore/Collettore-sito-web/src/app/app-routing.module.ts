import { GraficiComponent } from './grafici/grafici.component';
import { MappaComponent } from './mappa/mappa.component';
import { NgModule } from '@angular/core';
import { Routes, RouterModule } from '@angular/router';
import { TabelleComponent } from './tabelle/tabelle.component';

const routes: Routes = [
  { path: 'mappa', component: MappaComponent, data: { animation: 'isLeft' } },
  { path: 'grafici', component: GraficiComponent },
  { path: 'tabelle', component: TabelleComponent, data: { animation: 'isRight' } },
  { path: '', redirectTo: '/mappa', pathMatch: 'full'}
];

@NgModule({
  imports: [RouterModule.forRoot(routes)],
  exports: [RouterModule]
})
export class AppRoutingModule { }
