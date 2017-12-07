# Joc-pe-matricea-de-leduri-Spaceships

Joc pe matricea de LED-uri
Mihaela Nistor 352

Titlu: Spaceships

Descriere:
    O nava traverseaza spatiul cosmic si trebuie sa se fereasca de asteroizi sau sa ii distruga cu rachete pentru a-si face loc printre ei. Cu cat inainteaza mai mult, asteroizii incep sa fie mai desi, iar nava prinde din ce in ce mai multa viteza.

Implementare:
    Inainte de a incepe jocul, se afiseaza pe LCD un meniu cu doua linii: “Start!” si “Schimba nivel: 1”. Cu joystick-ul in sus si in jos se schimba linia din meniu, iar cu butonul se selecteaza. Daca e selectata prima linie incepe jocul, daca e selectata a doua creste nivelul pana la 7 apoi o ia din nou de la 1. In timpul jocului vor fi afisate: numarul de vieti, nivelul si scorul curent.

Coordonatele jucatorului sunt globale, la fel si matricile cu coordonatele obstacolelor si a rachetelor pentru a fi mai usor de verificat coliziunile.

Intr-o iteratie prin loop se parcurg cativa pasi:

    -Se verifica daca jocul e in curs sau e pe cale sa inceapa si se afiseaza meniul corespunzator
    -Se verifica daca jocul s-a terminat (numar vieti == 0), daca da, se afiseaza pe LCD scorul si nivelul la care s-a terminat jocul
    -Se ruleaza joc:
            - se calculeaza coordonatele jucatorului
            - se genereaza obstacole noi (numarul lor si distanta dintre ele depinde de nivel);
            - se deseneaza rachetele noi daca este apasat butonul sau joystick-ul
            - se deseneaza rachetele, obstacolele si jucatorul
        	- se verifica daca rachetele se intalnesc cu obstacolele; daca da, se sterge si racheta si obstacolul si se adauga 10   puncte la scor
        	- se verifica daca jucatorul a lovit vreun obstacol; daca da, se creeaza o explozie ( se aprind 25 de leduri, ce formeaza un patrat cu centrul in locul coliziunii); in locul exploziei dispar toate obstacolele si rachetele; se scade numarul de vieti
        	- se recalculeaza numarul de vieti, scorul, nivelul(dificultatea) : cand scorul este egal cu un multiplu de 1000 se trece la nivelul urmator si se adauga o viata  
    - Se sting LED-urile de pe matrice    
    - Se sterg datele de pe LCD

Testare:

    Meniu:
    
    Joystick:     -axa Ox:     parcurgere meniu
    Button:                    se selecteaza o linie din meniu            
                    
    Joc:
    Joystick:     -Ox, Oy      deplasare jucator
                  -button      lanseaza rachete

    Button:                    lanseaza rachete

