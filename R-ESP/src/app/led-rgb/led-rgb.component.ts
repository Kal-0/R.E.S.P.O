import { Component, EventEmitter, Output } from '@angular/core';
import { CommonModule } from '@angular/common';
import { FormsModule } from '@angular/forms';

@Component({
  selector: 'app-led-rgb',
  standalone: true,
  imports: [CommonModule, FormsModule],
  templateUrl: './led-rgb.component.html',
  styleUrl: './led-rgb.component.css'
})
export class LedRgbComponent {

  r: number = 0;
  g: number = 0;
  b: number = 0;

  @Output() corMudou = new EventEmitter<string>();

  get corHex(): string {
    const hex = (v: number) => v.toString(16).padStart(2, '0');
    return `#${hex(this.r)}${hex(this.g)}${hex(this.b)}`;
  }

  ngOnChanges() {
    this.emitirCor();
  }

  emitirCor() {
    this.corMudou.emit(this.corHex);
  }

  // Chame isso sempre que qualquer valor mudar
  atualizar() {
    this.emitirCor();
  }
}
