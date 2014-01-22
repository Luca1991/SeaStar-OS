SeaStar OS
==========

STATO ATTUALE: Implementato VFS base e supporto Fat12 FS (sola lettura) 

PROSSIMO TASK: Implementazione userland e syscall

SeaStar OS è un sistema operativo scritto da 0, verrà presentato come tesi di laurea
ed è stato scritto per scopi didattici e per capire in dettaglio come funziona un
sistema x86.
Ho scritto SeaStar OS tenendo a mente l'importanza della portabilità sulle diverse 
famiglie di architetture, pertanto tutti i files prettamente dipendeti da archittettura
x86 si trovano nella cartall Arch/x86.
Il resto del sistema dovrebbe essere portabile senza difficoltà.
SeaStar OS è ancora in fase embrionale.

Task critici:

* Gestione degli interrupt migliore (scrivere uno stub in asm per tutti gli interrupt)
* Librerie standard da migliorare e pulire (molte delle quali sono state prese dal web
e richiedono qualche ora di pulizia e ottimizzazione)
* usando la funzione kernelPrintf, il wildcard %s NON VERRA' ESEGUITO CORRETTAMENTE
(questo e' un bug critico da fixare il prima possibile)


ATTENZIONE: i problemi presenti nel codice sono accompagnati da un commento FIXME

NOTA: per compilare l'os occorre avere il compilatore i586-elf-gcc in path (se siete su linux a 64bit)

Luca D'Amico

PS: I suggerimenti e i commenti sono ben accetti !! Se volete aiutarmi speditemi 
una mail.

Special Thanks
--------------

Neon (Mike) for his great os-dev tutorials series

^Inuyasha^ (Ivan Gualandri) per tutti i consigli datomi su IRC

