# Pjäser

Alla pjäser karaktäriseras av ett int-tal. De definieras enligt följande:
* Bonde: 1
* Torn: 5
* Häst: 3
* Löpare: 4
* Drottning: 9
* Kung: 1000

Pjäsens färg urskiljs genom att tilldela svarta pjäser med den negativa motsvarigheten av ett tal.

# Schackbräda

En schackbräda är representerad av en tvådimensionell vector (från biblioteket <vector>). Den är uppbyggd med en vector som innehåller åtta andra vectorer. Dessa åtta vectorer innehåller i sin tur åtta platser åt datatypen int, där pjäserna kan lagras. Schackbrädet kan därför betraktas analogt med ett verkligt schackbräde eftersom data lagras enligt en 8x8 matris (8 rader, 8 kolumner) som sammanlagt uppgår till 64 platser (brickor). En specifik position på schackbrädet kan nås genom att anropa chessBoard[r][k], vilket då returnerar int-värdet av pjäsen som befinner sig på schackbrädets position rad r, kolumn k. Exempelvis kan man anropa chessBoard[2][5] för att ta reda på vad som befinner sig på rad 2, kolumn 5. Tomma brickor har int-värdet 0. 

Vanlig schacknotation använder siffrorna 1-8 för vertikal position och bokstäverna A-H för horisontell position. Sett ur perspektivet från spelaren med vita pjäser så ligger positionen A1 längst ner till vänster, medan positionen H8 ligger längst upp till höger. Detta avviker sig ifrån hur en vector fungerar, vari positionen längst upp till vänster är [0][0] och positionen längst ner till höger är [7][7]. För att förtydliga koden används definitioner av raderna (ranks) 1-8 och kolumnerna (files) A-H enligt följande:

* enum FILES { file_A, file_B, file_C, file_D, file_E, file_F, file_G, file_H };
* enum RANKS { rank_8, rank_7, rank_6, rank_5, rank_4, rank_3, rank_2, rank_1 };

Att skriva chessBoard[rank_1][file_A] blir då detsamma som att skriva chessBoard[7][0], vilket returnerar - liksom standardnotationen - pjäsen längst ner till vänster.

# Nomenklatur

* Ply - ett drag av en spelare.
* Tile - en bricka eller position på schackbrädet
* P/p - bonde (stor bokstav = vit färg)
* R/r - torn
* N/n - häst
* B/b - löpare
* Q/q - drottning
* K/k - Kung
* Castling/rockaddrag - specialdrag för kungapjäsen
* En passant - specialdrag för bondepjäsen

# Drag

Drag lagras som en sträng (från biblioteket <string>). Alla drag innehåller gemensamt vilken position pjäsen kom från samt vilken position den flyttat till. Ett drag från e2 till e4 betecknas då helt enkelt e2e4. Ifall draget sker med en bonde skrivs inte pjäsen ut i draget. Annars ifall draget skett med en annan pjäs inkluderas dess bokstav innan själva förflyttningen. T.ex. ifall en löpare av vit färg flyttar sig från c1 till e3 betecknas draget Bc1c3 (vore löparen svart - bc1c3). 

Ifall draget innebär ett slag mot en motståndares pjäs betecknas draget med ett suffix bestående av ett streck och bokstaven på den slagne pjäsen. T.ex. en drottning av vit färg på position f4 som slår ett torn på position f8 skulle betecknas som Qf4f8-r. 

Rockad (https://sv.wikipedia.org/wiki/Rockad) betecknas genom att ange kungens förflyttning följt av ett suffix bestående av ett streck och åt vilket håll rockaddraget sker åt. Det finns endast två sätt för varje färg att använda rockaddraget: Ke1g1, Ke1c1 för vit och ke8g8, ke8c8 för svart. Ke1g1 och ke8g8 innebär en förflyttning åt höger och noteras därigenom som Ke1g1-rcast, ke8g8-rcast (right castling). För Ke1c1 och ke8c8 gäller detsamma fastän med suffix lcast (left castling).

En passant (https://sv.wikipedia.org/wiki/En_passant) som är ett drag som slår motstånderens bonde, betecknas genom att ange förflyttning samt suffixet av ett streck följt av "enPa". Exempelvis betecknas en-passant-draget från e5 till f6 som e5f6-enPa.

# Generalisering av drag

* Förflyttning av bonde: startposition + slutposition (e2e4)
* Förflyttning av annan pjäs: Pjäs + startposition + slutposition (Bc1e3)
* Slag mot motståndares pjäs: Ovanstående motsvarighet + "-" + slagen pjäs (Bc1e3-n)
* Rockad: Kung + startposition + slutposition + "-" + förflyttningsriktning (Ke1g1-rcast)
* En passant: startposition + slutposition + "-" + enPassant (e5f6-enPa)

# Funktionen som kalkylerar drag

Det finns tolv olika funktioner som kalkylerar alla tillåtna drag (6 olika vita pjäser och 6 olika svarta pjäser). En funktion tar ett schackbräde som en parameter eller ingång och returnerar därefter en vector som innehåller strängar av alla tillåtna drag. I funktionen används en for-loop inuti en for-loop för att leta upp positionen på pjäsen vars drag ska returneras. Därefter beräknas alla tillåtna drag genom en rad olika villkorssatser. Ifall ett drag är tillåtet skapas en sträng av lämplig beteckning som sedan läggs i vectorn som returnerar alla de tillåtna dragen.

Utöver dessa funktioner finns även två andra funktioner av högre grad (en för vita pjäser, en för svarta) som returnerar alla tillåtna drag överhuvudtaget. Exempelvis kalkylerar funktionen för vita pjäser först alla tillåtna drag för alla vita pjäser med hjälp av de tolv "underfunktionerna", därefter samlas alla tillåtna drag i en kollektiv vector av strängar som sedan används för att returnera dragen. 
