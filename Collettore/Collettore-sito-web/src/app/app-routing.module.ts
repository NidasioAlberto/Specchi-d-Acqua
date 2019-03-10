import { GraficiComponent } from './grafici/grafici.component';
import { MappaComponent } from './mappa/mappa.component';
import { NgModule } from '@angular/core';
import { Routes, RouterModule } from '@angular/router';
import { TabelleComponent } from './tabelle/tabelle.component';
import { RecordComponent } from './tabelle/record/record.component';
import { BarchetteComponent } from './tabelle/barchette/barchette.component';
import { SensoriComponent } from './tabelle/sensori/sensori.component';

const routes: Routes = [
  { path: 'mappa', component: MappaComponent, data: { animation: 'isLeft' } },
  { path: 'grafici', component: GraficiComponent, data: { animation: 'isCenter'} },
  { path: 'tabelle', component: TabelleComponent, data: { animation: 'isRight' }, children: [
    { path: 'record', component: RecordComponent },
    { path: 'barchette', component: BarchetteComponent },
    { path: 'sensori', component: SensoriComponent }
  ] },
  { path: '', redirectTo: '/mappa', pathMatch: 'full'}
];

@NgModule({
  imports: [RouterModule.forRoot(routes)],
  exports: [RouterModule]
})
export class AppRoutingModule { }
