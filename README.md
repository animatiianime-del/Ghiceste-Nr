# 🎮 Joc de Ghicit Numărul (0-100) - Sistem Incorporat Arduino & Custom PCB

Proiect hardware-software dezvoltat pe arhitectură de **Automat de Stări Finite (FSM)** folosind un microcontroler Arduino Uno, un ecran LCD 1602, o tastatură matriceală 4x4, un buton mecanic de Start/Stop cu filtru de debounce, feedback acustico-vizual și persistență de date în memoria non-volatilă EEPROM pentru salvarea recordurilor de timp.

Proiectul include documentația schematică completă și design-ul plăcii de circuit imprimat (**PCB dublu-strat**) proiectat în KiCad, optimizat prin tehnica *Pin Swapping*.

---

## 🚀 Caracteristici Principale

* **Automat de Stări Finite (FSM):** Arhitectură software modulară fără blocaje (`STARE_OPRIT`, `STARE_JOC`, `STARE_VICTORIE`, `STARE_GAME_OVER`).
* **Control Optimizat Pini (Pin Swapping):** Re-mapare hardware a pinilor digitali și analogici pentru rutare PCB curată, paralelă și fără treceri (*vias*) inutile.
* **Ecran LCD 1602 (Mod 4 Biți):** Comandat integral de pe pinii analogici (`A0`–`A5`) folosiți în regim de pini digitali IO.
* **Tastatură Matriceală 4x4:** Rutează pe pinii digitali consecutivi (`D13`–`D6`) direct pe stratul superior de cupru.
* **Debounce Software:** Filtru pe butonul de Start/Stop (`D5`) fără rezistență externă (utilizează `INPUT_PULLUP` intern).
* **Persistență Date EEPROM:** Salvarea și citirea automată a celui mai bun timp de rezolvare (secunde).
* **Proiectare PCB (KiCad):** Circuit imprimat pe 2 straturi cu Plan de Masă (`GND` turnat pe `B.Cu`), validat 100% cu **0 erori DRC** (*Design Rules Check*).

---

## 🛠️ Componente Hardware

| Componentă | Specificație / Detalii | Cantitate |
| :--- | :--- | :---: |
| **Microcontroler** | Arduino Uno R3 (ATmega328P) | 1 |
| **Afișaj** | LCD 1602A (Mod de lucru 4 biți) | 1 |
| **Tastatură** | Matriceală 4x4 (Membrană / Push-buttons) | 1 |
| **Buton Control** | Push-Button 6x6mm (Start/Stop) | 1 |
| **Feedback Vizual** | LED Roșu 5mm + LED Verde 5mm | 2 |
| **Limitoresc Curent** | Rezistențe $220\,\Omega$ (pentru LED-uri) | 2 |
| **Feedback Acustic** | Buzzer Activ 5V | 1 |
| **Reglaj Contrast** | Potențiometru 10k$\Omega$ | 1 |

---

## 🔌 Schema de Conexiuni (Pinout Table Final)

### 1. Ecran LCD 1602 (Grupat pe Antetul Analogic)
| Pin LCD | Funcție LCD | Conexiune Arduino Uno | Observații |
| :--- | :--- | :--- | :--- |
| **Pin 1 / 16** | `VSS` / `K` (Catod LED) | **GND** | Plan de Masă |
| **Pin 2 / 15** | `VDD` / `A` (Anod LED) | **+5V** | Alimentare |
| **Pin 3** | `VO` (Contrast) | **Cursor Potențiometru (RV1)** | Reglaj manual |
| **Pin 4** | `RS` (Register Select) | **Pin A0** | I/O Digital |
| **Pin 5** | `R/W` (Read/Write) | **GND** | Setat permanent pe scriere |
| **Pin 6** | `E` (Enable) | **Pin A1** | Impuls ceas |
| **Pins 11-14** | `D4`, `D5`, `D6`, `D7` | **A2, A3, A4, A5** | Linii date (Mod 4 biți) |

### 2. Tastatură Matriceală 4x4 (Antetul Digital Superior)
| Pin Tastatură | Funcție | Pin Arduino Uno |
| :--- | :--- | :--- |
| **Rânduri (R1 – R4)** | Linii Matrice | **D13, D12, D11, D10** |
| **Coloane (C1 – C4)** | Coloane Matrice | **D9, D8, D7, D6** |

### 3. Periferice Auxiliare
| Periferic | Funcție | Pin Arduino Uno |
| :--- | :--- | :--- |
| **Buton Start/Stop** | Control stare joc | **D5** (INPUT_PULLUP la GND) |
| **LED Roșu** | Indicator Greșit / Game Over | **D4** (Seriat cu $220\,\Omega$ la GND) |
| **LED Verde** | Indicator Victorie | **D3** (Seriat cu $220\,\Omega$ la GND) |
| **Buzzer Activ** | Feedback acustico-sonor | **D2** (NPN / Direct la GND) |

---

## 📐 Arhitectură Software (Diagramă FSM)

```mermaid
stateDiagram-v2
    [*] --> STARE_OPRIT

    STARE_OPRIT --> STARE_JOC : "Apasare Buton D5 (Start)"
    
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

    STARE_JOC --> STARE_OPRIT : "Apasare Buton D5 (Stop)"
```

---

## 🎮 Comenzi Tastatură
* **`0 – 9`**: Introducerea numărului propus (0–100).
* **`#`**: **Enter** / Validarea răspunsului curent.
* **`*`**: **Clear** / Ștergerea numărului introdus.
* **`D`**: **Indiciu (Peek)** / Afișează numărul secret timp de 2 secunde pe ecran (Cost: **-2 vieți**).

---

## 🖥️ Proiectare PCB (KiCad Workflow)

1. **Schematic Design:** Verificat cu 0 erori la verificarea regulilor electrice (**ERC**).
2. **Pin Swapping:** Optimizare fizică a traseelor prin gruparea pinilor LCD pe antetul analogic și ai tastaturii pe cel digital.
3. **PCB Layout:**
   * Board pe 2 straturi (`F.Cu` și `B.Cu`).
   * Turnare de Plan de Masă complet (`GND Zone`) pe stratul inferior `B.Cu`.
   * Trasee de alimentare și semnal dimensionate corespunzător.
4. **DRC Validation:** Verified & Cleared (**0 Errors / 0 Warnings / 0 Unconnected Pads**).
