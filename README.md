
    Pentru rezolvarea acestei teme m-am folosit in mare parte de laboratorul 9.
    Toate fisierele sunt luate din laborator, in afara de client.c care este 
    refacut de la 0 si de parson.c si parson.h .
    Functionalitatea aplicatiei:
      -Clientul accepta urmatoarele comenzi pentru a interactiona cu serverul
REST API:
        ->register: Permite inregistrarea unui cont nou. Utilizatorul trebuie
sa introduca un username si o parola. Comanda trimite o cerere POST la server
pentru a crea un nou cont.

        ->login: Permite autentificarea intr-un cont existent. Utilizatorul
trebuie sa introduca username-ul si parola. Comanda trimite o cerere POST
la server pentru a autentifica utilizatorul si pentru a obtine un cookie de
sesiune.

        ->enter_library: Permite accesul la biblioteca dupa autentificare. 
Comanda trimite o cerere GET la server pentru a obtine un token JWT necesar
pentru accesul la resursele bibliotecii.

        ->get_books: Afiseaza toate cartile disponibile in biblioteca. Comanda
trimite o cerere GET la server si afiseaza o lista de carti in format JSON.

        ->get_book: Afiseaza detalii despre o carte specifica. Utilizatorul 
trebuie sa introduca ID-ul cartii. Comanda trimite o cerere GET la server
pentru a obtine detaliile cartii respective.

        ->add_book: Permite adaugarea unei carti noi in biblioteca. Utilizatorul
trebuie sa introduca titlul, autorul, genul, numarul de pagini si editorul. 
Comanda trimite o cerere POST la server pentru a adauga cartea in biblioteca.

        ->delete_book: Permite stergerea unei carti din biblioteca. Utilizatorul
trebuie sa introduca ID-ul cartii. Comanda trimite o cerere DELETE la server 
pentru a sterge cartea respectiva.

        ->logout: Permite deconectarea utilizatorului din cont. Comanda trimite 
o cerere GET la server pentru a invalida sesiunea curenta.

        ->exit: Iesi din aplicatie. Aceasta comanda inchide conexiunea si opreste clientul. 
