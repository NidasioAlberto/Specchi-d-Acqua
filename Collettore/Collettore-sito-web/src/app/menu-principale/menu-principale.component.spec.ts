import { async, ComponentFixture, TestBed } from '@angular/core/testing';

import { MenuPrincipaleComponent } from './menu-principale.component';

describe('MenuPrincipaleComponent', () => {
  let component: MenuPrincipaleComponent;
  let fixture: ComponentFixture<MenuPrincipaleComponent>;

  beforeEach(async(() => {
    TestBed.configureTestingModule({
      declarations: [ MenuPrincipaleComponent ]
    })
    .compileComponents();
  }));

  beforeEach(() => {
    fixture = TestBed.createComponent(MenuPrincipaleComponent);
    component = fixture.componentInstance;
    fixture.detectChanges();
  });

  it('should create', () => {
    expect(component).toBeTruthy();
  });
});
