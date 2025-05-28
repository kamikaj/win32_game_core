Projekt: win32_game_core	
---
Autor:	Kamil Sitarski (kamikaj)
---
Opis:	  Pierwszy krok w nauce renderowania 2D w czystym Win32 API (bez bibliotek zewnętrznych).
 		     Aplikacja tworzy okno i rysuje gradient w pamięci RAM, który następnie wyświetla na ekranie.
        Projekt oparty na serii "Handmade Hero" autorstwa Casey'ego Muratori'ego (Molly Rokcet).
---

## Cel projektu

Nauczyć się:
- Jak działa system okienkowy Windows (Win32 API)
- Jak ręcznie zarządzać pamięcią (bitmapa w RAM)
- Jak wyświetlać grafikę przez GDI (`StretchDIBits`)
- Jak przetwarzać komunikaty systemowe (message loop)

---

## Co robi ten program?

- Tworzy własne okno Windows bez żadnych frameworków.
- Obsługuje zdarzenia systemowe (resize, zamknięcie, itp.).
- Tworzy i zarządza bitmapą w pamięci (Device Independent Bitmap).
- Renderuje dynamiczny gradient kolorów (zielono-niebieski).
- Rysuje grafikę do RAM, a następnie wyświetla ją w oknie.

---


## Technologie

- Język: C++
- Win32 API
- GDI (Graphics Device Interface)
- Kompilator: MSVC / Clang / MinGW

---

## Czego się uczę

- Tworzenie okna przy pomocy `CreateWindowEx`
- Praca z komunikatami systemowymi (`WM_PAINT`, `WM_SIZE`, itd.)
- Renderowanie grafiki do bufora w pamięci i jej wyświetlanie
- Różnice między `PeekMessage` a `GetMessage`
- Architektura Little Endian i reprezentacja kolorów w pamięci

---

## Struktura pliku źródłowego

Plik zawiera:
- Definicje typów i aliasów (`uint8`, `uint32`, itd.)
- Makra ułatwiające kontrolę widoczności (`internal`, `local_persist`)
- Funkcje: `RenderWeirdGradient`, `Win32ResizeDIBSection`, `Win32UpdateWindow`
- Callback okna `Win32MainWindowCallback`
- Główna funkcja aplikacji `WinMain`

---

## Inspiracja

Projekt oparty na [Handmade Hero](https://handmadehero.org) – serii pokazującej jak stworzyć własny silnik i grę od zera, krok po kroku, z dokładnym zrozumieniem działania każdego elementu.

---

