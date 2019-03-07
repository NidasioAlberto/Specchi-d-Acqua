import { async, ComponentFixture, TestBed } from '@angular/core/testing';

import { BarchetteComponent } from './barchette.component';

describe('BarchetteComponent', () => {
  let component: BarchetteComponent;
  let fixture: ComponentFixture<BarchetteComponent>;

  beforeEach(async(() => {
    TestBed.configureTestingModule({
      declarations: [ BarchetteComponent ]
    })
    .compileComponents();
  }));

  beforeEach(() => {
    fixture = TestBed.createComponent(BarchetteComponent);
    component = fixture.componentInstance;
    fixture.detectChanges();
  });

  it('should create', () => {
    expect(component).toBeTruthy();
  });
});
