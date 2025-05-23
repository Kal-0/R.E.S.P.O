import { Component } from '@angular/core';
import { PainelControleComponent } from '../painel-controle/painel-controle.component';

@Component({
  selector: 'app-home',
  standalone: true,
  imports: [PainelControleComponent],
  templateUrl: './home.component.html',
  styleUrl: './home.component.css'
})
export class HomeComponent {

}
