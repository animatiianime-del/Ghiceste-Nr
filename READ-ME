# Joc de Ghicit Numărul (0-100) pe Microcontroler Arduino

Proiect dezvoltat pe arhitectură de Automat de Stări Finite (FSM) folosind o plăcuță Arduino Uno/Mega, ecran LCD 1602, tastatură matriceală 4x4, buton mecanic de Start/Stop, feedback acustic și vizual (LED-uri + Buzzer) și persistență de date în memoria non-volatilă EEPROM.

## 🛠️ Componente Hardware
- **Microcontroler:** Arduino Uno / Mega 2560
- **Afișaj:** LCD 1602 (Mod 4 biți)
- **Introducere date:** Tastatură matriceală 4x4
- **Control alimentare:** Buton push cu filtru software de debounce
- **Feedback vizual:** LED Verde (Victorie), LED Roșu (Eroare)
- **Feedback acustic:** Buzzer activ
- **Memorie:** EEPROM (Salvare Record Timp)

## 📐 Arhitectură Software (Diagramă FSM)

```mermaid
stateDiagram-v2
    [*] --> STARE_OPRIT

    STARE_OPRIT --> STARE_JOC : "Apasare Buton A3 (Start)"
    
    state STARE_JOC {
        [*] --> AsteptareTasta
        
        AsteptareTasta --> ProcesareCifra : "Taste 0-9"
        ProcesareCifra --> AsteptareTasta : "Adauga cifra"
        
        AsteptareTasta --> ResetareInput : "Tasta *"
        ResetareInput --> AsteptareTasta : "Sterge numar"
        
        AsteptareTasta --> VerificareIndiciu : "Tasta D"
        VerificareIndiciu --> AsteptareTasta : "Indiciu: -2 Veti"
        
        AsteptareTasta --> ValidareRaspuns : "Tasta #"
    }

    ValidareRaspuns --> STARE_VICTORIE : "Numar Corect"
    ValidareRaspuns --> VerificareVeti : "Numar Gresit"

    VerificareVeti --> STARE_JOC : "Veti > 0"
    VerificareVeti --> STARE_GAME_OVER : "Veti == 0"

    STARE_VICTORIE --> STARE_JOC : "Resetare si Salvare EEPROM"
    STARE_GAME_OVER --> STARE_JOC : "Resetare si Afisare Solutie"

    STARE_JOC --> STARE_OPRIT : "Apasare Buton A3 (Stop)"
```

## 🎮 Comenzi Tastatură
- `0 - 9`: Introducerea numărului
- `#`: Enter / Validare răspuns
- `*`: Ștergere cifră
- `D`: Indiciu / Trișare (Afișează numărul secret timp de 2 secunde, cost: 2 vieți)