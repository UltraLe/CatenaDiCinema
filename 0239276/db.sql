-- MySQL Workbench Forward Engineering

SET @OLD_UNIQUE_CHECKS=@@UNIQUE_CHECKS, UNIQUE_CHECKS=0;
SET @OLD_FOREIGN_KEY_CHECKS=@@FOREIGN_KEY_CHECKS, FOREIGN_KEY_CHECKS=0;
SET @OLD_SQL_MODE=@@SQL_MODE, SQL_MODE='TRADITIONAL,ALLOW_INVALID_DATES';

-- -----------------------------------------------------
-- Schema CatenaCinema
-- -----------------------------------------------------
DROP SCHEMA IF EXISTS `CatenaCinema` ;

-- -----------------------------------------------------
-- Schema CatenaCinema
-- -----------------------------------------------------
CREATE SCHEMA IF NOT EXISTS `CatenaCinema` DEFAULT CHARACTER SET utf8 ;
USE `CatenaCinema` ;

-- -----------------------------------------------------
-- Table `CatenaCinema`.`Cinema`
-- -----------------------------------------------------
DROP TABLE IF EXISTS `CatenaCinema`.`Cinema` ;

CREATE TABLE IF NOT EXISTS `CatenaCinema`.`Cinema` (
  `ID` INT NOT NULL,
  PRIMARY KEY (`ID`))
ENGINE = InnoDB;


-- -----------------------------------------------------
-- Table `CatenaCinema`.`Sala`
-- -----------------------------------------------------
DROP TABLE IF EXISTS `CatenaCinema`.`Sala` ;

CREATE TABLE IF NOT EXISTS `CatenaCinema`.`Sala` (
  `NumeroDiSala` INT NOT NULL,
  `NumeroDiPosti` INT NOT NULL,
  `Cinema` INT NOT NULL,
  PRIMARY KEY (`NumeroDiSala`, `Cinema`),
  CONSTRAINT `fk_Sala_Cinema1`
    FOREIGN KEY (`Cinema`)
    REFERENCES `CatenaCinema`.`Cinema` (`ID`)
    ON DELETE NO ACTION
    ON UPDATE NO ACTION)
ENGINE = InnoDB;

CREATE INDEX `fk_Sala_Cinema1_idx` ON `CatenaCinema`.`Sala` (`Cinema` ASC);


-- -----------------------------------------------------
-- Table `CatenaCinema`.`Posto`
-- -----------------------------------------------------
DROP TABLE IF EXISTS `CatenaCinema`.`Posto` ;

CREATE TABLE IF NOT EXISTS `CatenaCinema`.`Posto` (
  `Numero` INT NOT NULL,
  `Fila` VARCHAR(1) NOT NULL,
  `Sala` INT NOT NULL,
  `Cinema` INT NOT NULL,
  PRIMARY KEY (`Numero`, `Fila`, `Sala`, `Cinema`),
  CONSTRAINT `fk_Posto_Sala1`
    FOREIGN KEY (`Sala` , `Cinema`)
    REFERENCES `CatenaCinema`.`Sala` (`NumeroDiSala` , `Cinema`)
    ON DELETE NO ACTION
    ON UPDATE NO ACTION)
ENGINE = InnoDB;

CREATE INDEX `fk_Posto_Sala1_idx` ON `CatenaCinema`.`Posto` (`Sala` ASC, `Cinema` ASC);


-- -----------------------------------------------------
-- Table `CatenaCinema`.`Film`
-- -----------------------------------------------------
DROP TABLE IF EXISTS `CatenaCinema`.`Film` ;

CREATE TABLE IF NOT EXISTS `CatenaCinema`.`Film` (
  `Nome` VARCHAR(128) NOT NULL,
  `Durata` INT NOT NULL,
  `CasaCinematografica` VARCHAR(45) NOT NULL,
  PRIMARY KEY (`Nome`))
ENGINE = InnoDB;


-- -----------------------------------------------------
-- Table `CatenaCinema`.`Dipendente`
-- -----------------------------------------------------
DROP TABLE IF EXISTS `CatenaCinema`.`Dipendente` ;

CREATE TABLE IF NOT EXISTS `CatenaCinema`.`Dipendente` (
  `CodiceFiscale` VARCHAR(45) NOT NULL,
  `Nome` VARCHAR(45) NOT NULL,
  `Cognome` VARCHAR(45) NOT NULL,
  `Tipo` VARCHAR(45) NOT NULL,
  `Cinema` INT NOT NULL,
  PRIMARY KEY (`CodiceFiscale`),
  CONSTRAINT `fk_Dipendente_Cinema1`
    FOREIGN KEY (`Cinema`)
    REFERENCES `CatenaCinema`.`Cinema` (`ID`)
    ON DELETE NO ACTION
    ON UPDATE NO ACTION)
ENGINE = InnoDB;

CREATE INDEX `fk_Dipendente_Cinema1_idx` ON `CatenaCinema`.`Dipendente` (`Cinema` ASC);


-- -----------------------------------------------------
-- Table `CatenaCinema`.`Proiezione`
-- -----------------------------------------------------
DROP TABLE IF EXISTS `CatenaCinema`.`Proiezione` ;

CREATE TABLE IF NOT EXISTS `CatenaCinema`.`Proiezione` (
  `Data` DATE NOT NULL,
  `Ora` TIME NOT NULL,
  `IncassoTotale` INT NULL,
  `Film` VARCHAR(128) NOT NULL,
  `Sala` INT NOT NULL,
  `Dipendente` VARCHAR(45) NULL,
  `Cinema` INT NOT NULL,
  `Costo` INT NOT NULL,
  PRIMARY KEY (`Data`, `Film`, `Sala`, `Ora`, `Cinema`),
  CONSTRAINT `fk_Proiezione_Film1`
    FOREIGN KEY (`Film`)
    REFERENCES `CatenaCinema`.`Film` (`Nome`)
    ON DELETE NO ACTION
    ON UPDATE NO ACTION,
  CONSTRAINT `fk_Proiezione_Sala1`
    FOREIGN KEY (`Sala` , `Cinema`)
    REFERENCES `CatenaCinema`.`Sala` (`NumeroDiSala` , `Cinema`)
    ON DELETE NO ACTION
    ON UPDATE NO ACTION,
  CONSTRAINT `fk_Proiezione_Dipendente1`
    FOREIGN KEY (`Dipendente`)
    REFERENCES `CatenaCinema`.`Dipendente` (`CodiceFiscale`)
    ON DELETE NO ACTION
    ON UPDATE NO ACTION)
ENGINE = InnoDB;

CREATE INDEX `fk_Proiezione_Film1_idx` ON `CatenaCinema`.`Proiezione` (`Film` ASC);

CREATE INDEX `fk_Proiezione_Sala1_idx` ON `CatenaCinema`.`Proiezione` (`Sala` ASC, `Cinema` ASC);

CREATE INDEX `fk_Proiezione_Dipendente1_idx` ON `CatenaCinema`.`Proiezione` (`Dipendente` ASC);


-- -----------------------------------------------------
-- Table `CatenaCinema`.`Attore`
-- -----------------------------------------------------
DROP TABLE IF EXISTS `CatenaCinema`.`Attore` ;

CREATE TABLE IF NOT EXISTS `CatenaCinema`.`Attore` (
  `Nome` VARCHAR(45) NOT NULL,
  `Cognome` VARCHAR(45) NOT NULL,
  PRIMARY KEY (`Nome`, `Cognome`))
ENGINE = InnoDB;


-- -----------------------------------------------------
-- Table `CatenaCinema`.`AttoriNelFilm`
-- -----------------------------------------------------
DROP TABLE IF EXISTS `CatenaCinema`.`AttoriNelFilm` ;

CREATE TABLE IF NOT EXISTS `CatenaCinema`.`AttoriNelFilm` (
  `NomeAttore` VARCHAR(45) NOT NULL,
  `CognomeAttore` VARCHAR(45) NOT NULL,
  `NomeFilm` VARCHAR(128) NOT NULL,
  PRIMARY KEY (`NomeAttore`, `CognomeAttore`, `NomeFilm`),
  CONSTRAINT `fk_Attore_has_Film_Attore`
    FOREIGN KEY (`NomeAttore` , `CognomeAttore`)
    REFERENCES `CatenaCinema`.`Attore` (`Nome` , `Cognome`)
    ON DELETE NO ACTION
    ON UPDATE NO ACTION,
  CONSTRAINT `fk_Attore_has_Film_Film1`
    FOREIGN KEY (`NomeFilm`)
    REFERENCES `CatenaCinema`.`Film` (`Nome`)
    ON DELETE NO ACTION
    ON UPDATE NO ACTION)
ENGINE = InnoDB;

CREATE INDEX `fk_Attore_has_Film_Film1_idx` ON `CatenaCinema`.`AttoriNelFilm` (`NomeFilm` ASC);

CREATE INDEX `fk_Attore_has_Film_Attore_idx` ON `CatenaCinema`.`AttoriNelFilm` (`NomeAttore` ASC, `CognomeAttore` ASC);


-- -----------------------------------------------------
-- Table `CatenaCinema`.`Turno`
-- -----------------------------------------------------
DROP TABLE IF EXISTS `CatenaCinema`.`Turno` ;

CREATE TABLE IF NOT EXISTS `CatenaCinema`.`Turno` (
  `FasciaOraria` VARCHAR(10) NOT NULL,
  `Giorno` DATE NOT NULL,
  `Cinema` INT NOT NULL,
  PRIMARY KEY (`FasciaOraria`, `Giorno`, `Cinema`),
  CONSTRAINT `fk_Turno_Cinema1`
    FOREIGN KEY (`Cinema`)
    REFERENCES `CatenaCinema`.`Cinema` (`ID`)
    ON DELETE NO ACTION
    ON UPDATE NO ACTION)
ENGINE = InnoDB;

CREATE INDEX `fk_Turno_Cinema1_idx` ON `CatenaCinema`.`Turno` (`Cinema` ASC);


-- -----------------------------------------------------
-- Table `CatenaCinema`.`TurnoDelDipendente`
-- -----------------------------------------------------
DROP TABLE IF EXISTS `CatenaCinema`.`TurnoDelDipendente` ;

CREATE TABLE IF NOT EXISTS `CatenaCinema`.`TurnoDelDipendente` (
  `FasciaOrariaTurno` VARCHAR(10) NOT NULL,
  `GiornoTurno` DATE NOT NULL,
  `CodiceFiscaleDipendente` VARCHAR(45) NOT NULL,
  PRIMARY KEY (`FasciaOrariaTurno`, `GiornoTurno`, `CodiceFiscaleDipendente`),
  CONSTRAINT `fk_Turno_has_Dipendente_Turno1`
    FOREIGN KEY (`FasciaOrariaTurno` , `GiornoTurno`)
    REFERENCES `CatenaCinema`.`Turno` (`FasciaOraria` , `Giorno`)
    ON DELETE NO ACTION
    ON UPDATE NO ACTION,
  CONSTRAINT `fk_Turno_has_Dipendente_Dipendente1`
    FOREIGN KEY (`CodiceFiscaleDipendente`)
    REFERENCES `CatenaCinema`.`Dipendente` (`CodiceFiscale`)
    ON DELETE NO ACTION
    ON UPDATE NO ACTION)
ENGINE = InnoDB;

CREATE INDEX `fk_Turno_has_Dipendente_Dipendente1_idx` ON `CatenaCinema`.`TurnoDelDipendente` (`CodiceFiscaleDipendente` ASC);


-- -----------------------------------------------------
-- Table `CatenaCinema`.`CartaDiCredito`
-- -----------------------------------------------------
DROP TABLE IF EXISTS `CatenaCinema`.`CartaDiCredito` ;

CREATE TABLE IF NOT EXISTS `CatenaCinema`.`CartaDiCredito` (
  `Numero` VARCHAR(16) NOT NULL,
  `NomeIntestatario` VARCHAR(45) NOT NULL,
  `CognomeIntestatario` VARCHAR(45) NOT NULL,
  `MeseScadenza` INT UNSIGNED NOT NULL,
  `AnnoScadenza` INT UNSIGNED NOT NULL,
  `CVV` INT UNSIGNED NOT NULL,
  PRIMARY KEY (`Numero`))
ENGINE = InnoDB;


-- -----------------------------------------------------
-- Table `CatenaCinema`.`Biglietto`
-- -----------------------------------------------------
DROP TABLE IF EXISTS `CatenaCinema`.`Biglietto` ;

CREATE TABLE IF NOT EXISTS `CatenaCinema`.`Biglietto` (
  `CodiceDiPrenotazione` VARCHAR(45) NOT NULL,
  `Tipo` VARCHAR(45) NOT NULL,
  `NumeroPosto` INT NOT NULL,
  `FilaPosto` VARCHAR(1) NOT NULL,
  `DataProiezione` DATE NOT NULL,
  `Film` VARCHAR(128) NOT NULL,
  `SalaDellaProiezione` INT NOT NULL,
  `OraProiezione` TIME NOT NULL,
  `CinemaDellaProiezione` INT NOT NULL,
  `NumeroCarta` VARCHAR(16) NOT NULL,
  PRIMARY KEY (`CodiceDiPrenotazione`, `DataProiezione`, `Film`, `OraProiezione`, `NumeroPosto`, `FilaPosto`, `SalaDellaProiezione`, `CinemaDellaProiezione`),
  CONSTRAINT `fk_Biglietto_Posto1`
    FOREIGN KEY (`NumeroPosto` , `FilaPosto`)
    REFERENCES `CatenaCinema`.`Posto` (`Numero` , `Fila`)
    ON DELETE NO ACTION
    ON UPDATE NO ACTION,
  CONSTRAINT `fk_Biglietto_Proiezione1`
    FOREIGN KEY (`DataProiezione` , `Film` , `SalaDellaProiezione` , `OraProiezione` , `CinemaDellaProiezione`)
    REFERENCES `CatenaCinema`.`Proiezione` (`Data` , `Film` , `Sala` , `Ora` , `Cinema`)
    ON DELETE NO ACTION
    ON UPDATE NO ACTION,
  CONSTRAINT `fk_Biglietto_CartaDiCredito1`
    FOREIGN KEY (`NumeroCarta`)
    REFERENCES `CatenaCinema`.`CartaDiCredito` (`Numero`)
    ON DELETE NO ACTION
    ON UPDATE NO ACTION)
ENGINE = InnoDB;

CREATE INDEX `fk_Biglietto_Posto1_idx` ON `CatenaCinema`.`Biglietto` (`NumeroPosto` ASC, `FilaPosto` ASC);

CREATE INDEX `fk_Biglietto_Proiezione1_idx` ON `CatenaCinema`.`Biglietto` (`DataProiezione` ASC, `Film` ASC, `SalaDellaProiezione` ASC, `OraProiezione` ASC, `CinemaDellaProiezione` ASC);

CREATE INDEX `fk_Biglietto_CartaDiCredito1_idx` ON `CatenaCinema`.`Biglietto` (`NumeroCarta` ASC);

USE `CatenaCinema` ;

-- -----------------------------------------------------
-- procedure popolaSalePosti
-- -----------------------------------------------------

USE `CatenaCinema`;
DROP procedure IF EXISTS `CatenaCinema`.`popolaSalePosti`;

DELIMITER $$
USE `CatenaCinema`$$
CREATE PROCEDURE `popolaSalePosti`(in sistema_reale int)
BEGIN
	
    declare i int default 0;
    declare j int;
    declare k int;
    declare larghezzaSala int;
    declare fila varchar(1);
    declare cinema int;
    declare numSale int;
    declare numPosti int;
	declare cur cursor for select ID from Cinema;
    
    declare exit handler for sqlexception 
	begin
		rollback;
        resignal;
	end;
    
    #procedura usata per popolare il db, usata solo nel test, ed una sola volta,
    #non mi preoccupo del livello di isolamento
   
    # se ci sono già delle sale mi fermo
	if ((select NumeroDiSala from Sala where Cinema = '1' limit 1) is not null) then
		signal sqlstate '45001'
        set message_text = 'Delle sale sono gia presenti in db';
	end if;

	open cur;
    while i < (select count(*) from Cinema) do
    
		fetch cur into cinema;
        #ora a tale cinema inserisco un numero casuale di sale, da 6 a 10.
        select floor((10-6)*RAND()+6) into numSale;
        set j = 1;
        while j <= numSale do
        
			#ogni sala ha un numero di posti casuali, da 40 a 60
            select floor((60-40)*RAND()+40) into numPosti;
			insert into Sala values (j, numPosti, cinema);
			
            #procedo con la popolazione dei posti
            set fila = 'A';
            #larghezza delle sale (data dal numero di posti per fila, assumo vari da 7 a 10)
			select floor((10-7)*RAND()+7) into larghezzaSala;
            set k = 0;
            while k < numPosti do
            
                insert into Posto values (k%larghezzaSala+1, fila, j, cinema);
                set k = k +1;
                #se ho raggiunto la fine della fila, 'scalo dietro'. Es. dalla fila A passo alla B
                if (not k%larghezzaSala) then
					set fila = CHAR(ASCII(fila) + 1);
				end if;
                
            end while;
            set j = j + 1;
            
		end while;
        
        #per evitare lungo tempo di attesa, possibilita di scegliere se simulare sistema reale oppure no
        # set i = i + 1;
        if sistema_reale then
			set i = i +1;
		else
			set i = 100;
		end if;
        
	end while;
    close cur;
    
END$$

DELIMITER ;

-- -----------------------------------------------------
-- procedure aggiungiAttoriAlFilm
-- -----------------------------------------------------

USE `CatenaCinema`;
DROP procedure IF EXISTS `CatenaCinema`.`aggiungiAttoriAlFilm`;

DELIMITER $$
USE `CatenaCinema`$$
CREATE PROCEDURE `aggiungiAttoriAlFilm`(in nome_attore varchar(30), in cognome_attore varchar(20), in nome_film varchar(128))
BEGIN
    
	set transaction isolation level serializable;

	insert into AttoriNelFilm values (nome_attore, cognome_attore, nome_film);

END$$

DELIMITER ;

-- -----------------------------------------------------
-- procedure popolaAttoriNelFilm
-- -----------------------------------------------------

USE `CatenaCinema`;
DROP procedure IF EXISTS `CatenaCinema`.`popolaAttoriNelFilm`;

DELIMITER $$
USE `CatenaCinema`$$
CREATE PROCEDURE `popolaAttoriNelFilm`()
BEGIN
	
    declare i int default 0;
    declare j int;
    declare nomeFilm varchar(128);
    declare nomeAtt varchar(30);
    declare cognomeAtt varchar(20);
    declare num_attori int;
    declare cur cursor for select Nome from Film;
    
    declare exit handler for sqlexception
    begin	
		rollback;
		resignal;
	end;
    
    #procedura usata per popolare il db, usata solo nel test, ed una sola volta,
    #non mi preoccupo del livello di isolamento
    
	#se la tabella non è vuota, è preferibile usare la procedura 'AggiungiAttoriAlFilm'
    if ((select NomeAttore from AttoriNelFilm limit 1) is not null) then
		signal sqlstate '45001'
        set message_text = 'Dei record in AttoriNelFilm sono gia presenti in db';
	end if;
    
    #se non sono presenti dei record in Attore o Film, non posso far nulla
    if ((select Nome from Attore limit 1) is null) then
		signal sqlstate '45002'
		set message_text = 'Devi prima inserire degli attori';
	end if;
    if ((select Nome from Film limit 1) is null) then
		signal sqlstate '45003'
		set message_text = 'Devi prima inserire dei film';
	end if;
    
    #assegno a tutti i film in db un 'cast di attori protagonisti'
    #che varia da 6 a 10 attori protagonisti
    
    open cur;
	while (i < (select count(*) from Film)) do
		
		fetch cur into nomeFilm;
        select floor((10-6)*RAND()+6) into num_attori;
        set j = 0;
        while_att: while ( j < num_attori ) do
        
			select Nome, Cognome from Attore order by RAND() limit 1 into nomeAtt, cognomeAtt;
            #se e stato scelto lo stesso attore per lo stesso film ripeto
            if ((select A.Nome
				from Attore A join AttoriNelFilm ANF on A.Nome = ANF.NomeAttore and A.Cognome = ANF.CognomeAttore
                where Nome = nomeAtt and Cognome = cognomeAtt and ANF.NomeFilm = nomeFilm) is not null) then
                iterate while_att;
			end if;
            
            insert into AttoriNelFilm values (nomeAtt, cognomeAtt, nomeFilm);
			set j = j + 1;
        
        end while while_att;
        
		set i = i + 1;
		
    end while;
    close cur;
    

END$$

DELIMITER ;

-- -----------------------------------------------------
-- procedure popolaProiezioneSenzaProiezionista
-- -----------------------------------------------------

USE `CatenaCinema`;
DROP procedure IF EXISTS `CatenaCinema`.`popolaProiezioneSenzaProiezionista`;

DELIMITER $$
USE `CatenaCinema`$$
CREATE PROCEDURE `popolaProiezioneSenzaProiezionista`(in cinema int)
BEGIN

	declare i int default 0;
    declare j int;
    declare k int;
    #assumo che inizialmente ci siano 4 nuovi film di cui si 
    #vogliono fissare le proiezioni
    declare num_film int default 4;
    declare numProiezGiorn int;
    declare giornoCorrente varchar(10);
    declare costo_p int;
    declare sala_casuale int;
    declare ora_casuale TIME;
    declare dataCorrente DATE;
    declare finito int default false;
    declare nome_film varchar(128);
    declare durata_p int;
    declare cur_film cursor for select Nome, Durata from Film order by RAND() limit num_film;
	
    declare continue handler for not found set finito = true;
    declare exit handler for sqlexception
    begin
		rollback;
        resignal;
	end;
    
    #procedura usata per popolare il db, usata solo nel test, ed una sola volta,
    #non mi preoccupo del livello di isolamento
    
    #se sono già presenti delle proiezioni, mi fermo.
    #in questo caso aggiungerne di nuove con la procedura 'aggiungiProiezione'
    if ((select Ora from Proiezione limit 1) is not null) then
		signal sqlstate '45006'
		set message_text = 'La tabella Proiezione e gia stata popolata, usare aggiungiProiezione';
	end if;
    
    open cur_film;
	while_tag: while (i < num_film) do
		
        #prendo un film a caso
        fetch cur_film into nome_film, durata_p;
        
        if finito then
			leave while_tag;
        end if;
        
        select CURDATE() into dataCorrente;
        #e ne aggiungo un numero a caso di proiezioni giornaliere che va da 2 a 4
        #per 2 settimane lavorative, a partire dalla data corrente
        set j = 0;
        while_loop: while (j < 12) do
			
            select DATE_ADD(dataCorrente, interval 1 day) into dataCorrente;
            select DAYNAME(dataCorrente) into giornoCorrente;
			#se sono a lunedi' vado avanti, senza incrementare il contatore, il lunedi' assumo cinema chiuso
			if ((select giornoCorrente where giornoCorrente like 'Mon%') is not null) then
				iterate while_loop;
			end if;
			set j = j + 1;
            
            #per il giorno in cui mi trovo aggiungo o 2 o 4 proiezioni
            select floor((4-2)*RAND()+2) into numProiezGiorn;
            set k = 0;
            scegli_ora_sala: while k < numProiezGiorn do
				#scelgo un costo casuale che va dai 7 ai 10 euro
                select (FLOOR((11-7)*RAND()+7)) into costo_p;
                
				#scelgo un ora (di inizio film) casuale che vada dalle 16 alle 22 (film può avere durata massima di 2 ore e 10), intervallate di 100 minuti
                select (
					case FLOOR((RAND()*100)%5)
					when 0 then '16:00:00'
                    when 1 then '17:40:00'
                    when 2 then '19:20:00'
                    when 3 then '21:00:00'
                    when 4 then '22:40:00'
                    else 0 end)
				into ora_casuale;
				
                #seleziono una sala disponibile casuale
                select NumeroDiSala as SaleDisponibili	
				from Sala
				where Cinema = cinema and NumeroDiSala not in (
					select distinct S.NumeroDiSala
					from  Sala S join Proiezione P on S.NumeroDiSala = P.Sala join Film F on P.Film = F.Nome
					where S.Cinema = cinema and P.Data = dataCorrente 
					and ( 
						   (ADDTIME(ora_casuale, SEC_TO_TIME(durata_p*60)) >= P.Ora and ADDTIME(ora_casuale, SEC_TO_TIME(durata_p*60)) <= ADDTIME(P.Ora, SEC_TO_TIME(F.Durata*60)))
						or (ora_casuale >= P.Ora and ADDTIME(P.Ora, SEC_TO_TIME(F.Durata*60)) <= ADDTIME(ora_casuale, SEC_TO_TIME(durata_p*60)))
						or (P.Ora <= ora_casuale and ADDTIME(P.Ora, SEC_TO_TIME(F.Durata*60)) >= ADDTIME(ora_casuale, SEC_TO_TIME(durata_p*60)))
						or (ora_casuale <= P.Ora and ADDTIME(P.Ora, SEC_TO_TIME(F.Durata*60)) <= ADDTIME(ora_casuale, SEC_TO_TIME(durata_p*60)))
						))
				order by RAND() limit 1
                into sala_casuale;
                if sala_casuale is null then
					select 'Non ci sono sale disponibili';
					leave while_tag;
				end if;
                
                #se e' stata scela la stessa sala e lo stesso orario nello stesso giorno, ripeto
                if ((select Data from Proiezione
					where Data = dataCorrente and Film = nome_film and Ora = ora_casuale and Sala = sala_casuale and Cinema = Cinema)
					is not null) then
                    iterate scegli_ora_sala;
				end if;
                
                insert into Proiezione(Data, Ora, Film, Sala, Cinema, Costo)
                values (dataCorrente, ora_casuale, nome_film, sala_casuale, cinema, costo_p);
                
            set k = k + 1;    
            end while scegli_ora_sala;
            
        end while while_loop;
        
    set i = i + 1;
    end while while_tag;
    close cur_film;
    
        
END$$

DELIMITER ;

-- -----------------------------------------------------
-- procedure aggiungiProiezione
-- -----------------------------------------------------

USE `CatenaCinema`;
DROP procedure IF EXISTS `CatenaCinema`.`aggiungiProiezione`;

DELIMITER $$
USE `CatenaCinema`$$
CREATE PROCEDURE `aggiungiProiezione`(in data_p DATE, in ora_p TIME, in film varchar(128), in sala_p int, in cinema int, in proiezionista varchar(45), in costo_p int)
BEGIN
	
    declare durata_p int;
    
    declare exit handler for sqlexception
    begin
		rollback;
        resignal;
	end;
    
    #non voglio permettere nessun tipo di anomalia perchè l'aggiunta della proiezione
    #coinvolge la ricerca di sala e proiezionista ''liberi'' e innesca dei trigger per
    #la costruzione dei turni dei dipendenti, quindi una anomalia potrebbe
    #provocare il comportamento inaspettato di tale procedura
    
    set transaction isolation level serializable;
    
    start transaction;
    
    #estraggo la durata del film che si vuole proiettare
    select Durata from Film where Nome = film into durata_p;
    #verifico che la sala in input si effettivamente disponibile
    if (select sala_p where sala_p not in (
		select distinct S.NumeroDiSala
		from  Sala S join Proiezione P on S.NumeroDiSala = P.Sala join Film F on P.Film = F.Nome
		where S.Cinema = cinema and P.Data = data_p 
        and ( 
			(ADDTIME(ora_p, SEC_TO_TIME(durata_p*60)) >= P.Ora and ADDTIME(ora_p, SEC_TO_TIME(durata_p*60)) <= ADDTIME(P.Ora, SEC_TO_TIME(F.Durata*60)))
			or (ora_p >= P.Ora and ADDTIME(P.Ora, SEC_TO_TIME(F.Durata*60)) <= ADDTIME(ora_p, SEC_TO_TIME(durata_p*60)))
			or (P.Ora <= ora_p and ADDTIME(P.Ora, SEC_TO_TIME(F.Durata*60)) >= ADDTIME(ora_p, SEC_TO_TIME(durata_p*60)))
			or (ora_p <= P.Ora and ADDTIME(P.Ora, SEC_TO_TIME(F.Durata*60)) <= ADDTIME(ora_p, SEC_TO_TIME(durata_p*60))) ))is null) then
		signal sqlstate '45004'
		set message_text = 'Sala occupata, usa saleDisponibili per vedere quale sala scegliere';
	end if;
    
    #verifico che il dipendente scelto sia un proiezionista
    if (select proiezionista where proiezionista in (
		select CodiceFiscale 
        from Dipendente 
        where Tipo = 'Proiezionista' and Cinema = cinema) is null) then
        signal sqlstate '45006'
		set message_text = 'Il dipendente scelto non e nel cinema dato in input, o non e un proiezionista';
	end if;
    
    #verifico che il proiezionista selezionato sia effettivamente disponibile
    if (select proiezionista where proiezionista in (
		select distinct CodiceFiscale
        from Dipendente D join Proiezione P on D.CodiceFiscale = P.Dipendente and D.Cinema = P.Cinema join Film F on P.Film = F.Nome
        where P.Data = data_p and D.Tipo = 'Proiezionista'
        and ( 
			(ADDTIME(ora_p, SEC_TO_TIME(durata_p*60)) >= P.Ora and ADDTIME(ora_p, SEC_TO_TIME(durata_p*60)) <= ADDTIME(P.Ora, SEC_TO_TIME(F.Durata*60)))
			or (ora_p >= P.Ora and ADDTIME(P.Ora, SEC_TO_TIME(F.Durata*60)) <= ADDTIME(ora_p, SEC_TO_TIME(durata_p*60)))
			or (P.Ora <= ora_p and ADDTIME(P.Ora, SEC_TO_TIME(F.Durata*60)) >= ADDTIME(ora_p, SEC_TO_TIME(durata_p*60)))
			or (ora_p <= P.Ora and ADDTIME(P.Ora, SEC_TO_TIME(F.Durata*60)) <= ADDTIME(ora_p, SEC_TO_TIME(durata_p*60))) ))is not null) then
        signal sqlstate '45005'
		set message_text = 'Proiezionista occupato, usa proiezionistiDisponibili per vedere quale proiezionista scegliere';
	end if;
    
    insert into Proiezione (Data, Ora, Film, Sala, Dipendente, Cinema, Costo)
	values (data_p, ora_p, film, sala_p, proiezionista, cinema, costo_p);
        
    commit;
END$$

DELIMITER ;

-- -----------------------------------------------------
-- procedure saleDisponibili
-- -----------------------------------------------------

USE `CatenaCinema`;
DROP procedure IF EXISTS `CatenaCinema`.`saleDisponibili`;

DELIMITER $$
USE `CatenaCinema`$$
CREATE PROCEDURE `saleDisponibili`(in cinema int, in ora_p TIME, in data_p DATE, in durata_p int)
BEGIN

	set transaction isolation level repeatable read;

	#1)da tutte le sale del cinema in input
	select NumeroDiSala as SaleDisponibili	
    from Sala
	where Cinema = cinema and NumeroDiSala not in (
		#2)tolgo le sale in cui ho una proiezione in quella data a quell'ora
		select distinct S.NumeroDiSala
		from  Sala S join Proiezione P on S.NumeroDiSala = P.Sala join Film F on P.Film = F.Nome
		where S.Cinema = cinema and P.Data = data_p 
        and ( 
			(ADDTIME(ora_p, SEC_TO_TIME(durata_p*60)) >= P.Ora and ADDTIME(ora_p, SEC_TO_TIME(durata_p*60)) <= ADDTIME(P.Ora, SEC_TO_TIME(F.Durata*60)))
			or (ora_p >= P.Ora and ADDTIME(P.Ora, SEC_TO_TIME(F.Durata*60)) <= ADDTIME(ora_p, SEC_TO_TIME(durata_p*60)))
			or (P.Ora <= ora_p and ADDTIME(P.Ora, SEC_TO_TIME(F.Durata*60)) >= ADDTIME(ora_p, SEC_TO_TIME(durata_p*60)))
			or (ora_p <= P.Ora and ADDTIME(P.Ora, SEC_TO_TIME(F.Durata*60)) <= ADDTIME(ora_p, SEC_TO_TIME(durata_p*60))) ));
END$$

DELIMITER ;

-- -----------------------------------------------------
-- procedure proiezionistiDisponibili
-- -----------------------------------------------------

USE `CatenaCinema`;
DROP procedure IF EXISTS `CatenaCinema`.`proiezionistiDisponibili`;

DELIMITER $$
USE `CatenaCinema`$$
CREATE PROCEDURE `proiezionistiDisponibili`(in cinema int, in ora_p TIME,in data_p DATE, in durata_p int)
BEGIN

	set transaction isolation level repeatable read;
	
    #1)da tutti i proiezionisti di un determinato cinema
    select CodiceFiscale as DipendenteDisponibile
	from Dipendente
    where Tipo = 'Proiezionista' and Cinema = cinema and CodiceFiscale not in(
		#2)toglo quelli occupati nell'ora e data in input, piu' la durata
        select distinct CodiceFiscale
        from Dipendente D join Proiezione P on D.CodiceFiscale = P.Dipendente and D.Cinema = P.Cinema join Film F on P.Film = F.Nome
        where P.Data = data_p and D.Tipo = 'Proiezionista'
        and ( 
			(ADDTIME(ora_p, SEC_TO_TIME(durata_p*60)) >= P.Ora and ADDTIME(ora_p, SEC_TO_TIME(durata_p*60)) <= ADDTIME(P.Ora, SEC_TO_TIME(F.Durata*60)))
			or (ora_p >= P.Ora and ADDTIME(P.Ora, SEC_TO_TIME(F.Durata*60)) <= ADDTIME(ora_p, SEC_TO_TIME(durata_p*60)))
			or (P.Ora <= ora_p and ADDTIME(P.Ora, SEC_TO_TIME(F.Durata*60)) >= ADDTIME(ora_p, SEC_TO_TIME(durata_p*60)))
			or (ora_p <= P.Ora and ADDTIME(P.Ora, SEC_TO_TIME(F.Durata*60)) <= ADDTIME(ora_p, SEC_TO_TIME(durata_p*60))) ));
    
END$$

DELIMITER ;

-- -----------------------------------------------------
-- procedure popolaProiezione
-- -----------------------------------------------------

USE `CatenaCinema`;
DROP procedure IF EXISTS `CatenaCinema`.`popolaProiezione`;

DELIMITER $$
USE `CatenaCinema`$$
CREATE PROCEDURE `popolaProiezione`(in cinema int, in sistema_reale int, in num_film int)
BEGIN

	declare i int;
    declare j int;
    declare k int;
    #assumo che inizialmente ci siano 4 nuovi film di cui si 
    #vogliono fissare le proiezioni
    declare costo_p int;
    declare numProiezGiorn int;
    declare giornoCorrente varchar(10);
    declare sala_casuale int;
    declare ora_casuale TIME;
    declare proiezionista_casuale varchar(45);
    declare dataCorrente DATE;
    declare durata_p int;
    declare finito int default false;
    declare nome_film varchar(128);
    declare cur_film cursor for select Nome, Durata from Film order by RAND() limit num_film;
    
    declare exit handler for sqlexception
    begin
		rollback;
        resignal;
	end;
    
    #procedura usata per popolare il db, usata solo nel test, ed una sola volta,
    #non mi preoccupo del livello di isolamento
    
    #se sono già presenti delle proiezioni, mi fermo.
    #in questo caso aggiungerne di nuove con la procedura aggiungiProiezione
    if ((select Ora from Proiezione where Cinema = cinema limit 1) is not null) then
		signal sqlstate '45006'
		set message_text = 'La tabella Proiezione e gia stata popolata per il cinema inserito, usare aggiungiProiezione';
	end if;

	if sistema_reale then
		set cinema = 1;
	end if;
    
    while (cinema <= (select count(*) from Cinema)) do
		select cinema;
        set i = 0;
        open cur_film;
		while_tag: while (i < num_film) do
			
			#prendo un film a caso
			fetch cur_film into nome_film, durata_p;
			
			select CURDATE() into dataCorrente;
			#e ne aggiungo un numero a caso di proiezioni giornaliere che va da 2 a 4
			#per 2 settimane lavorative, a partire dalla data corrente + un giorno
			set j = 0;
			while_loop: while (j < 12) do
				
				select DATE_ADD(dataCorrente, interval 1 day) into dataCorrente;
				select DAYNAME(dataCorrente) into giornoCorrente;
				#se sono a lunedi' vado avanti, senza incrementare il contatore, il lunedi' assumo cinema chiuso
				if ((select giornoCorrente where giornoCorrente like 'Mon%') is not null) then
					iterate while_loop;
				end if;
				set j = j + 1;
				
				#per il giorno in cui mi trovo aggiungo o 2 o 4 proiezioni
				select floor((4-2)*RAND()+2) into numProiezGiorn;
				set k = 0;
                set sala_casuale = 1;
				scegli_ora_sala: while k < numProiezGiorn do
					#scelgo un costo casuale che va dai 7 ai 10 euro
					select (FLOOR((11-7)*RAND()+7)) into costo_p;
				
					#scelgo un ora (di inizio film) casuale che vada dalle 16 alle 22 (film può avere durata massima di 2 ore e 10), intervallate di 100 minuti
					select (
						case FLOOR((RAND()*100)%5)
						when 0 then '16:00:00'
						when 1 then '17:40:00'
						when 2 then '19:20:00'
						when 3 then '21:00:00'
						when 4 then '22:40:00'
						else 0 end)
					into ora_casuale;
					
					#e una sala disponibile casuale
					select SS.NumeroDiSala	
					from Sala SS
					where SS.Cinema = cinema and SS.NumeroDiSala not in (
						select distinct S.NumeroDiSala
						from  Sala S join Proiezione P on S.NumeroDiSala = P.Sala join Film F on P.Film = F.Nome
						where S.Cinema = cinema and P.Data = dataCorrente 
						and ( 
							   (ADDTIME(ora_casuale, SEC_TO_TIME(durata_p*60)) >= P.Ora and ADDTIME(ora_casuale, SEC_TO_TIME(durata_p*60)) <= ADDTIME(P.Ora, SEC_TO_TIME(F.Durata*60)))
							or (ora_casuale >= P.Ora and ADDTIME(P.Ora, SEC_TO_TIME(F.Durata*60)) <= ADDTIME(ora_casuale, SEC_TO_TIME(durata_p*60)))
							or (P.Ora <= ora_casuale and ADDTIME(P.Ora, SEC_TO_TIME(F.Durata*60)) >= ADDTIME(ora_casuale, SEC_TO_TIME(durata_p*60)))
							or (ora_casuale <= P.Ora and ADDTIME(P.Ora, SEC_TO_TIME(F.Durata*60)) <= ADDTIME(ora_casuale, SEC_TO_TIME(durata_p*60)))
							))
					order by RAND() limit 1
					into sala_casuale;
					
					#se e' stata scela la stessa sala e lo stesso orario nello stesso giorno, ripeto
					if ((select P.Data from Proiezione P
						where P.Data = dataCorrente and P.Film = nome_film and P.Ora = ora_casuale and P.Sala = sala_casuale and P.Cinema = cinema)
						is not null) then
						iterate scegli_ora_sala;
					end if;
					
					if sala_casuale is null then
						select 'Non ci sono sale disponibili';
						leave while_tag;
					end if;
					#devo selezionare un proiezionista casuale disponibile,
					select DD.CodiceFiscale
					from Dipendente DD
					where DD.Tipo = 'Proiezionista' and DD.Cinema = cinema and DD.CodiceFiscale not in(
						select distinct D.CodiceFiscale
						from Dipendente D join Proiezione P on D.CodiceFiscale = P.Dipendente and D.Cinema = P.Cinema join Film F on P.Film = F.Nome
						where P.Data = dataCorrente and D.Tipo = 'Proiezionista'
						and ( 
							   (ADDTIME(ora_casuale, SEC_TO_TIME(durata_p*60)) >= P.Ora and ADDTIME(ora_casuale, SEC_TO_TIME(durata_p*60)) <= ADDTIME(P.Ora, SEC_TO_TIME(F.Durata*60)))
							or (ora_casuale >= P.Ora and ADDTIME(P.Ora, SEC_TO_TIME(F.Durata*60)) <= ADDTIME(ora_casuale, SEC_TO_TIME(durata_p*60)))
							or (P.Ora <= ora_casuale and ADDTIME(P.Ora, SEC_TO_TIME(F.Durata*60)) >= ADDTIME(ora_casuale, SEC_TO_TIME(durata_p*60)))
							or (ora_casuale <= P.Ora and ADDTIME(P.Ora, SEC_TO_TIME(F.Durata*60)) <= ADDTIME(ora_casuale, SEC_TO_TIME(durata_p*60)))
							))
					order by RAND() limit 1
					into proiezionista_casuale;
					if proiezionista_casuale is null then
						select 'Non ci sono proiezionisti disponibili';
						leave while_tag;
					end if;
                    select cinema, j, k;
					insert into Proiezione(Data, Ora, Film, Sala, Dipendente, Cinema, Costo)
					values (dataCorrente, ora_casuale, nome_film, sala_casuale, proiezionista_casuale, cinema, costo_p);
				
				set k = k + 1;
				end while scegli_ora_sala;
				
			end while while_loop;
			
		set i = i + 1;
		end while while_tag;
    
		if sistema_reale then
			set cinema = cinema + 1;
		else
			set cinema = 100;
		end if;
        close cur_film;
        
    end while;
    
        
END$$

DELIMITER ;

-- -----------------------------------------------------
-- procedure aggiungiProiezioneSenzaProiezionista
-- -----------------------------------------------------

USE `CatenaCinema`;
DROP procedure IF EXISTS `CatenaCinema`.`aggiungiProiezioneSenzaProiezionista`;

DELIMITER $$
USE `CatenaCinema`$$
CREATE PROCEDURE `aggiungiProiezioneSenzaProiezionista`(in data_p DATE, in ora_p TIME, in film varchar(128), in cinema int, in sala int, in costo_p int)
BEGIN
	
    declare durata_p int;
    declare exit handler for sqlexception
    begin
		rollback;
        resignal;
    end;
    
    #non voglio permettere nessun tipo di anomalia perchè l'aggiunta della proiezione
    #coinvolge la ricerca di sale ''libere''
    #quindi una anomalia potrebbe
    #provocare il comportamento inaspettato di tale procedura
    
    set transaction isolation level serializable;
    
    start transaction;
    select Durata from Film where Nome = film into durata_p;
    #verifico che la sala in input si effettivamente disponibile
    if (select sala where sala not in (
		select distinct S.NumeroDiSala
		from  Sala S join Proiezione P on S.NumeroDiSala = P.Sala join Film F on P.Film = F.Nome
		where S.Cinema = cinema and P.Data = data_p 
        and ( 
			(ADDTIME(ora_p, SEC_TO_TIME(durata_p*60)) >= P.Ora and ADDTIME(ora_p, SEC_TO_TIME(durata_p*60)) <= ADDTIME(P.Ora, SEC_TO_TIME(F.Durata*60)))
			or (ora_p >= P.Ora and ADDTIME(P.Ora, SEC_TO_TIME(F.Durata*60)) <= ADDTIME(ora_p, SEC_TO_TIME(durata_p*60)))
			or (P.Ora <= ora_p and ADDTIME(P.Ora, SEC_TO_TIME(F.Durata*60)) >= ADDTIME(ora_p, SEC_TO_TIME(durata_p*60)))
			or (ora_p <= P.Ora and ADDTIME(P.Ora, SEC_TO_TIME(F.Durata*60)) <= ADDTIME(ora_p, SEC_TO_TIME(durata_p*60)))  ))is null) then
		signal sqlstate '45004'
		set message_text = 'Sala occupata, usa saleDisponibili per vedere quale sala scegliere';
	end if;

		insert into Proiezione(Data, Ora, Film, Sala, Cinema, Costo)
        values (data_p, ora_p, film, sala, cinema, costo_p);
    commit;
END$$

DELIMITER ;

-- -----------------------------------------------------
-- procedure aggiornaProiezionistaDellaProiezione
-- -----------------------------------------------------

USE `CatenaCinema`;
DROP procedure IF EXISTS `CatenaCinema`.`aggiornaProiezionistaDellaProiezione`;

DELIMITER $$
USE `CatenaCinema`$$
CREATE PROCEDURE `aggiornaProiezionistaDellaProiezione`(in proiezionista varchar(45), in data_p DATE, in ora_p TIME, in film varchar(128), in sala int, in cinema int)
BEGIN
	#prima dell'aggiunta del proiezionista alla proiezione,
    #assumo che il gestore abbia consultato i proiezionisti disponibili
    #con la procedura 'proiezionistiDisponibili'
    declare durata_p int;
    declare exit handler for sqlexception
    begin
		rollback;
        resignal;
	end;
    
    set transaction isolation level serializable;
    start transaction;
    select Durata from Film where Nome = film into durata_p;
    #verifico che il dipendente scelto sia un proiezionista
    if (select proiezionista where proiezionista in (
		select CodiceFiscale 
        from Dipendente 
        where Tipo = 'Proiezionista' and Cinema = cinema) is null) then
        signal sqlstate '45006'
		set message_text = 'Il dipendente scelto non e nel cinema dato in input, o non e un proiezionista';
	end if;
    
    #verifico che il proiezionista selezionato sia effettivamente disponibile
    if (select proiezionista where proiezionista in (
		select distinct D.CodiceFiscale
        from Dipendente D join Proiezione P on D.CodiceFiscale = P.Dipendente and D.Cinema = P.Cinema join Film F on P.Film = F.Nome
        where P.Data = data_p and D.Tipo = 'Proiezionista'
        and ( 
			(ADDTIME(ora_p, SEC_TO_TIME(durata_p*60)) >= P.Ora and ADDTIME(ora_p, SEC_TO_TIME(durata_p*60)) <= ADDTIME(P.Ora, SEC_TO_TIME(F.Durata*60)))
			or (ora_p >= P.Ora and ADDTIME(P.Ora, SEC_TO_TIME(F.Durata*60)) <= ADDTIME(ora_p, SEC_TO_TIME(durata_p*60)))
			or (P.Ora <= ora_p and ADDTIME(P.Ora, SEC_TO_TIME(F.Durata*60)) >= ADDTIME(ora_p, SEC_TO_TIME(durata_p*60)))
			or (ora_p <= P.Ora and ADDTIME(P.Ora, SEC_TO_TIME(F.Durata*60)) <= ADDTIME(ora_p, SEC_TO_TIME(durata_p*60)))  ))is not null) then
        signal sqlstate '45005'
		set message_text = 'Proiezionista occupato, usa proiezionistiDisponibili per vedere quale proiezionista scegliere';
	end if;
    
    #aggiungo il proiezionista alla proiezione
    update Proiezione P
    set Dipendente = proiezionista
    where P.Cinema = cinema and P.Data = data_p and P.Ora = ora and P.Film = film and P.Sala = sala;
    
    commit;
    
END$$

DELIMITER ;

-- -----------------------------------------------------
-- procedure calendarioTurni
-- -----------------------------------------------------

USE `CatenaCinema`;
DROP procedure IF EXISTS `CatenaCinema`.`calendarioTurni`;

DELIMITER $$
USE `CatenaCinema`$$
CREATE PROCEDURE `calendarioTurni`(in cinema int)
BEGIN
	declare data_corrente DATE default CURDATE();
    
    #voglio evitare le letture sporche e inconsistenti che potrebbero derivare
    #dalla chiamata di tale procedura contemporaneamente alla modifica di un tutno di un dipendente

    set transaction isolation level repeatable read;
    
    #stampa i turni a partire  dalla data corrente fino la domenica piu' vicina
    week_loop: loop
    
        select TD.FasciaOrariaTurno as FasciaOraria, TD.GiornoTurno as Giorno, TD.CodiceFiscaleDipendente as Dipendente
        from TurnoDelDipendente TD join Dipendente D on TD.CodiceFiscaleDipendente = D.CodiceFiscale
        where D.Cinema = cinema and TD.GiornoTurno = data_corrente;
        
        #se sono arrivato a domenica, mi fermo
        if (DAYNAME(data_corrente) like 'Sun%') then
			leave week_loop;
		end if;
        
        #altrimenti incremento il giorno
        set data_corrente = DATE_ADD(data_corrente, interval 1 day);
        
    end loop week_loop;
    
END$$

DELIMITER ;

-- -----------------------------------------------------
-- procedure postiDisponibiliDellaProiezione
-- -----------------------------------------------------

USE `CatenaCinema`;
DROP procedure IF EXISTS `CatenaCinema`.`postiDisponibiliDellaProiezione`;

DELIMITER $$
USE `CatenaCinema`$$
CREATE PROCEDURE `postiDisponibiliDellaProiezione`(in data_p DATE, in ora_p TIME, in film varchar(128), in sala int, in cinema int)
BEGIN
	
    #voglio evitare le letture sporche e le letture inconsistenti che potrebbero
    #non far funzionare come dovuto tale procedura
	
	set transaction isolation level repeatable read;

	#solo se la proiezione esiste vermante
	if ((select count(*)
		from Proiezione P
		where P.Data = data_p and P.Ora = ora_p and P.Film = film and P.Sala = sala and P.Cinema = cinema) > 0) then
		
        select P.Fila, P.Numero, (case 
									when ((select CodiceDiPrenotazione
										   from Biglietto
											where CinemaDellaProiezione = cinema and SalaDellaProiezione = sala and
													OraProiezione = ora_p and DataProiezione = data_p and Film = film and
														NumeroPosto = P.Numero and FilaPosto = P.Fila) is null
											or
											(select CodiceDiPrenotazione
											 from Biglietto
											  where CinemaDellaProiezione = cinema and SalaDellaProiezione = sala and
													OraProiezione = ora_p and DataProiezione = data_p and Film = film and
														NumeroPosto = P.Numero and FilaPosto = P.Fila and Tipo = 'Annullato') is not null) then '0'
									else '1'
									end
									) as Occupato
		from Posto P
		where P.Cinema = cinema and P.Sala = sala
		order by P.Fila asc;
    end if;
    
END$$

DELIMITER ;

-- -----------------------------------------------------
-- procedure confermaBiglietto
-- -----------------------------------------------------

USE `CatenaCinema`;
DROP procedure IF EXISTS `CatenaCinema`.`confermaBiglietto`;

DELIMITER $$
USE `CatenaCinema`$$
CREATE PROCEDURE `confermaBiglietto`(in codice varchar(45))
BEGIN

	declare exit handler for sqlexception
    begin
		rollback;
        resignal;
	end;
    
    #scelta del livello di isolamento come per la procedura 'annullaBiglietto'
    
    set transaction isolation level serializable;

	start transaction;
	
    #verifico l'esistenza del biglietto da annullare
    if (select CodiceDiPrenotazione 
		from Biglietto
		where CodiceDiPrenotazione = codice) is null then
        signal sqlstate '45011'
		set message_text = 'CodiceDiPrenotazione NON trovato';
	end if;
    
    #verifico che il biglietto non sia di tipo 'Annullato'
    if (select CodiceDiPrenotazione 
		from Biglietto
		where CodiceDiPrenotazione = codice and Tipo = 'Annullato') is not null then
        signal sqlstate '45014'
		set message_text = 'Questo biglietto e stato annullato';
	end if;
	
    #verifico che il biglietto non sia di tipo 'Confermato', non posso confermarlo 2 volte
    if (select CodiceDiPrenotazione 
		from Biglietto
		where CodiceDiPrenotazione = codice and Tipo = 'Confermato') is not null then
        signal sqlstate '45015'
		set message_text = 'Biglietto gia confermato';
	end if;
    
    update Biglietto set Tipo = 'Confermato' where CodiceDiPrenotazione = codice;
    
    commit;
    
END$$

DELIMITER ;

-- -----------------------------------------------------
-- procedure annullaBiglietto
-- -----------------------------------------------------

USE `CatenaCinema`;
DROP procedure IF EXISTS `CatenaCinema`.`annullaBiglietto`;

DELIMITER $$
USE `CatenaCinema`$$
CREATE PROCEDURE `annullaBiglietto`(in codice varchar(45))
BEGIN

	declare ora_p TIME;
    declare data_p DATE;

	declare exit handler for sqlexception
    begin
		rollback;
        resignal;
	end;
    
    #voglio evitare tutte le anomalie, potrei incorrere in lettura inconsistennte ed aggiornamento fantasma
    #(ad esempio se procedura chiamata contemporaneamente ad un report, per contare la tipologia dei biglietti in un mese)
    #se non mettessi il massimo livello di isolamento
    
    set transaction isolation level serializable;

	start transaction;
	
    #verifico l'esistenza del biglietto da annullare
    if (select CodiceDiPrenotazione 
		from Biglietto
		where CodiceDiPrenotazione = codice) is null then
        signal sqlstate '45011'
		set message_text = 'CodiceDiPrenotazione NON trovato';
	end if;
    
    #verifico che il biglietto non sia di tipo 'Confermato', sono questi posso annullare
    if (select CodiceDiPrenotazione 
		from Biglietto
		where CodiceDiPrenotazione = codice and Tipo = 'Confermato') is not null then
        signal sqlstate '45012'
		set message_text = 'Puoi annullare solo biglietto di tipo Confermato';
	end if;
	
    select OraProiezione, DataProiezione from Biglietto where CodiceDiPrenotazione = codice into ora_p, data_p;
    
	#verifico che non mi trovo nella mezzora prima di inizio del film
    if ((CURDATE() = data_p and ADDTIME(ora_p, -CURTIME()) < '00:30:00') or
				CURDATE() > data_p) then
		signal sqlstate '45013'
		set message_text = 'NON puoi annullare il biglietto prima di mezzora dall inizio del film o dopo la scadenza dello stesso';
	end if;
    
    update Biglietto set Tipo = 'Annullato' where CodiceDiPrenotazione = codice;
    
    commit;

END$$

DELIMITER ;

-- -----------------------------------------------------
-- procedure proiezioniDisponibili
-- -----------------------------------------------------

USE `CatenaCinema`;
DROP procedure IF EXISTS `CatenaCinema`.`proiezioniDisponibili`;

DELIMITER $$
USE `CatenaCinema`$$
CREATE PROCEDURE `proiezioniDisponibili`(in film varchar(128), in cinema int)
BEGIN

    # mostro tutte le proiezioni a partire dalla data corrente in poi,
    # con ora di inizio maggiore o uguale all ora attuale,
    
    # voglio evitare letture sporche ed inconsistenti
    
    set transaction isolation level repeatable read;
    
    select P.Data, P.Ora, P.Sala
    from Proiezione P
    where P.Data >= CURDATE() and P.Ora >= CURTIME() and P.Film = film and P.Cinema = cinema;
    

END$$

DELIMITER ;

-- -----------------------------------------------------
-- procedure popolaBiglietti
-- -----------------------------------------------------

USE `CatenaCinema`;
DROP procedure IF EXISTS `CatenaCinema`.`popolaBiglietti`;

DELIMITER $$
USE `CatenaCinema`$$
CREATE PROCEDURE `popolaBiglietti`(in cinema int, in sistema_reale int, in num_proiezioni int)
BEGIN
	#procedura che aggiunge dei biglietti casuali alle proiezioni di un cinema
    #se sono presenti dei biglietti nel db, meglio chiamare acquistaBiglietto
    
    declare i int;
    declare j int;
    declare data_p DATE;
    declare ora_p TIME;
    declare num_bigl int;
    declare capienza_sala int;
    declare numero_posto int;
    declare fila_posto varchar(1);
    declare nomeFilm varchar(128);
    declare sala int;
    
    declare exit handler for sqlexception
    begin
		rollback;
        resignal;
	end;
    
    #procedura usata per popolare il db, usata solo nel test, ed una sola volta,
    #non mi preoccupo del livello di isolamento
    
    if (select count(*) 
		from Biglietto where CinemaDellaProiezione = cinema) > 0 then
        signal sqlstate '45016'
		set message_text = 'Dei biglietti sono gia presenti in db. Popola con acquistaBiglietto';
    end if;
    
    if sistema_reale = 1 then
		set cinema = 1;
	end if;
    
    if num_proiezioni > 12 then
		signal sqlstate '45016'
		set message_text = 'NON puo essere num_proiezioni > 12';
    end if;
    
    while cinema <= (select count(*) from Cinema) do
		
        set i = 0;
        #1)scelgo num_proiezioni a cui far associare i biglietti
		while_proi: while i < num_proiezioni do
			#2)estraggo i dati della proiezione casuale nelle variabili dichiarate,
			select P.Data, P.Ora, P.Film, P.Sala
			from Proiezione P
			where P.Data >= CURDATE() and P.Ora >= CURTIME() and P.Film = film and P.Cinema = cinema
			order by RAND() limit 1 into data_p, ora_p, nomeFilm, sala;
            
            #se ha gia dei biglietti associati ritento,
            if(select CodiceDiPrenotazione
				from Biglietto
				where CinemaDellaProiezione = cinema and SalaDellaProiezione = sala and
					OraProiezione = ora_p and DataProiezione = data_p and Film = film limit 1) is not null then
				iterate while_proi;
			end if;
            
				#3)associo tale proiezione ad un numero casuale di billietti,
                #   che va dal 5% al 90% della capienza della sala
                select S.NumeroDiPosti from Sala S where S.NumeroDiSala = sala and S.Cinema = cinema into capienza_sala;
                select FLOOR((((RAND()*(90-5))+5)/100)*capienza_sala) into num_bigl;
                
                set j = 0;
                while j < num_bigl do
					
                    #4)estraggo un numero e fila di posto casuale, tra quelli disponibili
                    select P.Fila, P.Numero
					from Posto P
					where P.Cinema = cinema and P.Sala = sala and ((select CodiceDiPrenotazione
															from Biglietto
															where CinemaDellaProiezione = cinema and SalaDellaProiezione = sala and
																OraProiezione = ora_p and DataProiezione = data_p and Film = nomeFilm and
																	NumeroPosto = P.Numero and FilaPosto = P.Fila) is null
													or
															(select CodiceDiPrenotazione
															 from Biglietto
															  where CinemaDellaProiezione = cinema and SalaDellaProiezione = sala and
																	OraProiezione = ora_p and DataProiezione = data_p and Film = nomeFilm and
																		NumeroPosto = P.Numero and FilaPosto = P.Fila and Tipo = 'Annullato') is not null)
					order by RAND() limit 1
                    into fila_posto, numero_posto;
                    
                    #per completare la procedura di acquisto biglietto un cliente dovrebbe inserire i dati relativi alla
					#sua carta di credito. Inseriti gli stessi dati relativi alla CC per tutti i biglietto inseriti in fase di test
                    
                    call acquistaBiglietto(cinema, sala, data_p, ora_p, nomeFilm, numero_posto, fila_posto, '5255294813425786', 'Test', 'Tester', 2, 22, 884,  @cdp);
					set j = j + 1;
					
                end while;
                
			set i = i + 1;
            
		end while while_proi;
	
		if sistema_reale = 1 then
			set cinema = cinema + 1;
		else
			set cinema = 100;
		end if;
        
    end while;
    
END$$

DELIMITER ;

-- -----------------------------------------------------
-- procedure acquistaBiglietto
-- -----------------------------------------------------

USE `CatenaCinema`;
DROP procedure IF EXISTS `CatenaCinema`.`acquistaBiglietto`;

DELIMITER $$
USE `CatenaCinema`$$
CREATE PROCEDURE `acquistaBiglietto`(in cinema int, in sala int, in data_p DATE, in ora_p TIME, in film varchar(128), in numero_posto int, in fila_posto varchar(1), in numero varchar(16), in nomeIntest varchar(45), in cognomeIntest varchar(45), in meseScad int, in annoScad int, in cvv int, out codice_di_p varchar(45))
BEGIN

	declare codice varchar(45);
	
    declare exit handler for sqlexception
    begin
		rollback;
        resignal;
	end;
    
    declare exit handler for 1062
	begin
		rollback;
        signal sqlstate '45014'
		set message_text = 'Sei stato battuto sul tempo. Qualcuno ha appena occupato il tuo stesso posto.';
	end;
    
    set transaction isolation level serializable;
    
    start transaction;
    
    #prime di procedere controllo che la carta di credito sia presente nel db,
    #se così non fosse devo insrirla prima di continuare la procedura di acquisto del biglietto
    
    if (not cartaEsistente(`numero`)) then
		insert into CartaDiCredito values(numero, nomeIntest, cognomeIntest, meseScad, annoScad, cvv);
	end if;
    
    set codice = concat(data_p);
    set codice = concat(codice, cinema);
    set codice = concat(codice, ora_p);
    set codice = concat(codice, sala);
    set codice = concat(codice, CURTIME());
    set codice = concat(codice, fila_posto);
    set codice = concat(codice, numero_posto);
		
		#verifico che la proiezione inserit esista
    if( select P.Cinema 
		from Proiezione P
		where P.Data = data_p and P.Ora = ora_p and P.Film = film and P.Cinema = cinema and P.Sala = sala
        
        and
			#e che i posti inseriti siano effettivamente disponibili
        (fila_posto, numero_posto) in (select P.Fila, P.Numero
					from Posto P
					where P.Cinema = cinema and P.Sala = sala and ((select B.CodiceDiPrenotazione
															from Biglietto B
															where B.CinemaDellaProiezione = cinema and B.SalaDellaProiezione = sala and
																B.OraProiezione = ora_p and B.DataProiezione = data_p and B.Film = film and
																	B.NumeroPosto = P.Numero and B.FilaPosto = P.Fila) is null
													or
															(select B.CodiceDiPrenotazione
															 from Biglietto B
															  where B.CinemaDellaProiezione = cinema and B.SalaDellaProiezione = sala and
																	B.OraProiezione = ora_p and B.DataProiezione = data_p and B.Film = film and
																		B.NumeroPosto = P.Numero and B.FilaPosto = P.Fila and B.Tipo = 'Annullato') is not null))) is not null then
        
    
		insert into Biglietto values (codice, 'AcquistatoNonConfermato', numero_posto, fila_posto, data_p, film, sala, ora_p, cinema, numero);
		select codice into codice_di_p;
	end if;
    
    commit;
    
END$$

DELIMITER ;

-- -----------------------------------------------------
-- procedure simulaPostProiezione
-- -----------------------------------------------------

USE `CatenaCinema`;
DROP procedure IF EXISTS `CatenaCinema`.`simulaPostProiezione`;

DELIMITER $$
USE `CatenaCinema`$$
CREATE PROCEDURE `simulaPostProiezione`(in cinema int, in sala int, in film varchar(128), in data_p DATE, in ora_p TIME)
BEGIN

	#procedura usata per test,
    #per simulare l'acquisto e conferma e annullamento di biglietti diuna proiezione
    
	
    declare rand int;
    declare codice varchar(45);
    declare num_bigl int;
    declare i int default 0;
    
    declare cur cursor for select CodiceDiPrenotazione
	from Biglietto
	where CinemaDellaProiezione = cinema and SalaDellaProiezione = sala and Film = film
				and DataProiezione = data_p and OraProiezione = ora_p;
    
    declare exit handler for sqlexception
    begin
		rollback;
        resignal;
	end;
    
    select count(CodiceDiPrenotazione)
	from Biglietto
	where CinemaDellaProiezione = cinema and SalaDellaProiezione = sala and Film = film
				and DataProiezione = data_p and OraProiezione = ora_p into num_bigl;
    
    #verifico che ci siano dei biglietti associati alla proiezione
    if num_bigl = 0 then
		signal sqlstate '45017'
		set message_text = 'Nessun biglietto presente per la prenotazione in input';
    end if;
	
    open cur;
    while (i < num_bigl) do
		fetch cur into codice;
		set rand = FLOOR(RAND()*101);
		if( 0 <= rand and rand <= 5 ) then
			call annullaBiglietto(codice);
		end if;
		
		if(6 <= rand and rand <= 90) then
			call confermaBiglietto(codice);
		end if;
		set i = i + 1;
    
    end while;
    close cur;

END$$

DELIMITER ;
SET SQL_MODE = '';
GRANT USAGE ON *.* TO Gestore;
 DROP USER Gestore;
SET SQL_MODE='TRADITIONAL,ALLOW_INVALID_DATES';
CREATE USER 'Gestore' IDENTIFIED BY 'gestore';

GRANT ALL ON TABLE CatenaCinema.* TO 'Gestore';
GRANT ALL ON CatenaCinema.* TO 'Gestore';
SET SQL_MODE = '';
GRANT USAGE ON *.* TO Cliente;
 DROP USER Cliente;
SET SQL_MODE='TRADITIONAL,ALLOW_INVALID_DATES';
CREATE USER 'Cliente' IDENTIFIED BY 'cliente';

GRANT SELECT ON TABLE `CatenaCinema`.`Cinema` TO 'Cliente';
GRANT SELECT ON TABLE `CatenaCinema`.`Sala` TO 'Cliente';
GRANT SELECT ON TABLE `CatenaCinema`.`Posto` TO 'Cliente';
GRANT SELECT ON TABLE `CatenaCinema`.`Proiezione` TO 'Cliente';
GRANT SELECT ON TABLE `CatenaCinema`.`Film` TO 'Cliente';
GRANT SELECT ON TABLE `CatenaCinema`.`Attore` TO 'Cliente';
GRANT SELECT ON TABLE `CatenaCinema`.`AttoriNelFilm` TO 'Cliente';
GRANT ALL ON procedure `CatenaCinema`.`acquistaBiglietto` TO 'Cliente';
GRANT ALL ON procedure `CatenaCinema`.`annullaBiglietto` TO 'Cliente';
GRANT ALL ON procedure `CatenaCinema`.`postiDisponibiliDellaProiezione` TO 'Cliente';
GRANT ALL ON procedure `CatenaCinema`.`proiezioniDisponibili` TO 'Cliente';
SET SQL_MODE = '';
GRANT USAGE ON *.* TO Visitatore;
 DROP USER Visitatore;
SET SQL_MODE='TRADITIONAL,ALLOW_INVALID_DATES';
CREATE USER 'Visitatore' IDENTIFIED BY 'visitatore';

GRANT SELECT ON TABLE `CatenaCinema`.`Cinema` TO 'Visitatore';
GRANT SELECT ON TABLE `CatenaCinema`.`Sala` TO 'Visitatore';
GRANT SELECT ON TABLE `CatenaCinema`.`Posto` TO 'Visitatore';
GRANT SELECT ON TABLE `CatenaCinema`.`Proiezione` TO 'Visitatore';
GRANT SELECT ON TABLE `CatenaCinema`.`Film` TO 'Visitatore';
GRANT SELECT ON TABLE `CatenaCinema`.`Attore` TO 'Visitatore';
GRANT SELECT ON TABLE `CatenaCinema`.`AttoriNelFilm` TO 'Visitatore';
GRANT ALL ON procedure `CatenaCinema`.`proiezioniDisponibili` TO 'Visitatore';
SET SQL_MODE = '';
GRANT USAGE ON *.* TO Maschera;
 DROP USER Maschera;
SET SQL_MODE='TRADITIONAL,ALLOW_INVALID_DATES';
CREATE USER 'Maschera' IDENTIFIED BY 'maschera';

GRANT ALL ON TABLE `CatenaCinema`.`Biglietto` TO 'Maschera';
GRANT ALL ON procedure `CatenaCinema`.`confermaBiglietto` TO 'Maschera';

SET SQL_MODE=@OLD_SQL_MODE;
SET FOREIGN_KEY_CHECKS=@OLD_FOREIGN_KEY_CHECKS;
SET UNIQUE_CHECKS=@OLD_UNIQUE_CHECKS;

-- -----------------------------------------------------
-- Data for table `CatenaCinema`.`Cinema`
-- -----------------------------------------------------
START TRANSACTION;
USE `CatenaCinema`;
INSERT INTO `CatenaCinema`.`Cinema` (`ID`) VALUES (1);
INSERT INTO `CatenaCinema`.`Cinema` (`ID`) VALUES (2);
INSERT INTO `CatenaCinema`.`Cinema` (`ID`) VALUES (3);
INSERT INTO `CatenaCinema`.`Cinema` (`ID`) VALUES (4);
INSERT INTO `CatenaCinema`.`Cinema` (`ID`) VALUES (5);
INSERT INTO `CatenaCinema`.`Cinema` (`ID`) VALUES (6);
INSERT INTO `CatenaCinema`.`Cinema` (`ID`) VALUES (7);
INSERT INTO `CatenaCinema`.`Cinema` (`ID`) VALUES (8);
INSERT INTO `CatenaCinema`.`Cinema` (`ID`) VALUES (9);
INSERT INTO `CatenaCinema`.`Cinema` (`ID`) VALUES (10);

COMMIT;

USE `CatenaCinema`;

DELIMITER $$

USE `CatenaCinema`$$
DROP TRIGGER IF EXISTS `CatenaCinema`.`Proiezione_BEFORE_INSERT` $$
USE `CatenaCinema`$$
CREATE DEFINER = CURRENT_USER TRIGGER `CatenaCinema`.`Proiezione_BEFORE_INSERT` BEFORE INSERT ON `Proiezione` FOR EACH ROW
BEGIN
	
    declare pom varchar(10) default '16-20';
    declare ser varchar(10) default '20-24';
    declare entrambi int default 0;
    declare maschereDisponibili int;
    declare maschereDisponibiliSerali int;
    declare maschereDisponibiliPomeridiane int;
    declare mascheraCasuale varchar(45);
    declare durata_p int;
    declare fasciaOraria varchar(10);
    #se un proiezionista viene associato ad una proiezione, si deve verificare che
    #quel giorno a quell'ora sia presente in 'TurnoDelDipendente', altrimenti deve essere aggiunto
    #e dunque che quel giorno (data della proiezione) in una certa fascia oraria(da determinare in base 
    #all'ora della proiezione) deve essere presente una certa tupla un 'Turno'
    
    select F.Durata
    from Film F
    where F.Nome = NEW.Film
    into durata_p;
    
    #identifico la facia oraria in base all'ora della proiezione
    if ('16:00:00' <= NEW.Ora and NEW.Ora <= '20:00:00') then
		set fasciaOraria = pom;
	end if;
    if ('20:00:00' <= NEW.Ora and NEW.Ora <= '24:00:00') then
		set fasciaOraria = ser;
	end if;
    #se ho una proiezione che si sovrappone tra i 2 turni
    if (NEW.Ora <= '20:00:00'  and '20:00:00' <= ADDTIME(NEW.Ora, SEC_TO_TIME(durata_p*60))) then
		set entrambi = 1;
    end if;
    if ('24:00:00' < NEW.Ora or NEW.Ora < '16:00:00') then
		signal sqlstate '45009'
		set message_text = 'Ora della proiezione non valida, deve essere compresa tra le 16 e le 24';
	end if;
    
	if NEW.Dipendente is not null then
		if not entrambi then
			#prima verifico che sia presente nella tabella Turno la tupla:
			if ((select FasciaOraria
				from Turno T
				where T.Giorno = NEW.Data and T.Cinema = NEW.Cinema and T.FasciaOraria = fasciaOraria)
				is null) then
				#se non è presente, la creo
				insert into Turno values (fasciaOraria, NEW.Data, NEW.Cinema);
			end if;
			#se e presente devo verificare che il dipendente (se e stato selezionato per la proiezione
			#allora e sicuramente libero per come funzionano le procedure sviluppate) sia associato a quel turno
			#e se non lo fosse lo associo
			if ((select CodiceFiscaleDipendente
				from TurnoDelDipendente TD
				where TD.CodiceFiscaleDipendente = NEW.Dipendente and TD.GiornoTurno = NEW.Data and TD.FasciaOrariaTurno = fasciaOraria)
				is null )then
				insert into TurnoDelDipendente
				values (fasciaOraria, NEW.Data, NEW.Dipendente);
			end if;
            
            #faccio in modo che ci siano almeno 2 maschere differenti per la fascia oraria in cui ho la proiezione
            select count(distinct TD.CodiceFiscaleDipendente)
			from TurnoDelDipendente TD right join Dipendente D on TD.CodiceFiscaleDipendente = D.CodiceFiscale
			where TD.GiornoTurno = NEW.Data and D.Cinema = NEW.Cinema and TD.FasciaOrariaTurno = fasciaOraria and D.Tipo = 'Maschera'
            into maschereDisponibili;
            #per tante volte quante sono le maschere mancanti per arrivare a 2
            #aggiungo al turno una maschera a caso disponibile.
            #definisco una maschera disponibile per un cinema se: 
            #1)lavora per quel cinema
			#2)non ha un turno associato alla fascia oraria della proiezione
            while maschereDisponibili < 2 do
				
                select CodiceFiscale 
                from Dipendente
                where Tipo = 'Maschera' and Cinema = NEW.Cinema and CodiceFiscale not in(
					select D.CodiceFiscale
					from Dipendente D left join TurnoDelDipendente TD on D.CodiceFiscale = TD.CodiceFiscaleDipendente
							and D.Cinema = NEW.Cinema
					where TD.FasciaOrariaTurno = fasciaOraria and TD.GiornoTurno = NEW.Data and D.Tipo = 'Maschera')
				order by RAND() limit 1 into mascheraCasuale;
                
                insert into TurnoDelDipendente
                values (fasciaOraria, NEW.Data, mascheraCasuale);
                
                set maschereDisponibili = maschereDisponibili +1;
            
            end while;			
		#se il proiezionista deve gestire una proiezione tale per cui gli occupa entrambi i turni della giornata:
		else
			#devo verificare che sia presente sia il turno pomeridiano che quello serale
			if ((select FasciaOraria
				from Turno T
				where T.Giorno = NEW.Data and T.Cinema = NEW.Cinema and T.FasciaOraria = pom)
				is null) then
				insert into Turno values (pom, NEW.Data, NEW.Cinema);
			end if;
			if ((select FasciaOraria
				from Turno T
				where T.Giorno = NEW.Data and T.Cinema = NEW.Cinema and T.FasciaOraria = ser)
				is null) then
				insert into Turno values (ser, NEW.Data, NEW.Cinema);
			end if;
			#resta da verificare che il dipendente scelto per la gestione della proiezione
			#sia associato ai turni sia serale che pomeridiano
			if ((select CodiceFiscaleDipendente
				from TurnoDelDipendente TD
				where TD.CodiceFiscaleDipendente = NEW.Dipendente and TD.GiornoTurno = NEW.Data and TD.FasciaOrariaTurno = pom)
				is null )then
				insert into TurnoDelDipendente
				values (pom, NEW.Data, NEW.Dipendente);
			end if;
			if ((select CodiceFiscaleDipendente
				from TurnoDelDipendente TD
				where TD.CodiceFiscaleDipendente = NEW.Dipendente and TD.GiornoTurno = NEW.Data and TD.FasciaOrariaTurno = ser)
				is null )then
				insert into TurnoDelDipendente
				values (ser, NEW.Data, NEW.Dipendente);
			end if;
            #stessa cosa per le maschere, almeno 2 per il turno di sera e quello pomeridiano
            select count(distinct TD.CodiceFiscaleDipendente)
			from TurnoDelDipendente TD right join Dipendente D on TD.CodiceFiscaleDipendente = D.CodiceFiscale
			where TD.GiornoTurno = NEW.Data and D.Cinema = NEW.Cinema and TD.FasciaOrariaTurno = ser and D.Tipo = 'Maschera'
            into maschereDisponibiliSerali;
            
            select count(distinct TD.CodiceFiscaleDipendente)
			from TurnoDelDipendente TD right join Dipendente D on TD.CodiceFiscaleDipendente = D.CodiceFiscale
			where TD.GiornoTurno = NEW.Data and D.Cinema = NEW.Cinema and TD.FasciaOrariaTurno = pom and D.Tipo = 'Maschera'
            into maschereDisponibiliPomeridiane;
            
            while maschereDisponibiliSerali < 2 do
				
                select CodiceFiscale 
                from Dipendente
                where Tipo = 'Maschera' and Cinema = NEW.Cinema and CodiceFiscale not in(
					select D.CodiceFiscale
					from Dipendente D left join TurnoDelDipendente TD on D.CodiceFiscale = TD.CodiceFiscaleDipendente
							and D.Cinema = NEW.Cinema
					where TD.FasciaOrariaTurno = ser and TD.GiornoTurno = NEW.Data and D.Tipo = 'Maschera')
				order by RAND() limit 1 into mascheraCasuale;
                
                insert into TurnoDelDipendente
                values (ser, NEW.Data, mascheraCasuale);
                
                set maschereDisponibiliSerali = maschereDisponibiliSerali +1;
            
            end while;
            while maschereDisponibiliPomeridiane < 2 do
				
                select CodiceFiscale 
                from Dipendente
                where Tipo = 'Maschera' and Cinema = NEW.Cinema and CodiceFiscale not in(
					select D.CodiceFiscale
					from Dipendente D left join TurnoDelDipendente TD on D.CodiceFiscale = TD.CodiceFiscaleDipendente
							and D.Cinema = NEW.Cinema
					where TD.FasciaOrariaTurno = pom and TD.GiornoTurno = NEW.Data and D.Tipo = 'Maschera')
				order by RAND() limit 1 into mascheraCasuale;
                
                insert into TurnoDelDipendente
                values (pom, NEW.Data, mascheraCasuale);
                
                set maschereDisponibiliPomeridiane = maschereDisponibiliPomeridiane +1;
            
            end while;
		end if;
	end if;
    #se e stata inserita una proiezione senza specificare il dipendente
    #non faccio nulla
    
END$$


USE `CatenaCinema`$$
DROP TRIGGER IF EXISTS `CatenaCinema`.`Proiezione_AFTER_UPDATE` $$
USE `CatenaCinema`$$
CREATE DEFINER = CURRENT_USER TRIGGER `CatenaCinema`.`Proiezione_AFTER_UPDATE` AFTER UPDATE ON `Proiezione` FOR EACH ROW
BEGIN

	declare pom varchar(10) default '16-20';
    declare ser varchar(10) default '20-24';
    declare fasciaOraria varchar(10);
    declare entrambi int default 0;
    declare durata_p int;
    
    declare maschereDisponibili int;
    declare maschereDisponibiliSerali int;
    declare maschereDisponibiliPomeridiane int;
    declare mascheraCasuale varchar(45);

	if (OLD.Dipendente is not null) then
    
		select F.Durata
		from Film F
		where F.Nome = OLD.Film
		into durata_p;
		
		if ('16:00:00' <= OLD.Ora and OLD.Ora <= '20:00:00') then
			set fasciaOraria = pom;
		end if;
		if ('20:00:00' <= OLD.Ora and OLD.Ora <= '24:00:00') then
			set fasciaOraria = ser;
		end if;
		#se ho una proiezione che si sovrappone tra i 2 turni
		if (OLD.Ora <= '20:00:00'  and '20:00:00' <= ADDTIME(OLD.Ora, SEC_TO_TIME(durata_p*60))) then
		set entrambi = 1;
		end if;
		
        # se il proiezionista non ha associate altre proiezioni per quel giorno a quella fascia oraria
        # gli cancello il turno, o entrambi i turni, qualora la proiezione che verra occupata gli occupi
        # entrambi i turni
        if not entrambi then
        
			if ((select count(*) 
				from Proiezione P join Film F on P.Film = F.Nome
                where P.Ora <> OLD.Ora and P.Data = OLD.Data and P.Dipendente = OLD.Dipendente and P.Cinema = OLD.Cinema and 
						# calcolo la facia oraria per quel film
						fasciaOraria = (case
											# se in quel giorno il proiezionista ha un altro film che gli occupa
											# entrambi i turni, non voglio cancellarglieli
											when P.Ora <= '20:00:00' and '20:00:00' <= ADDTIME(P.Ora, SEC_TO_TIME(F.Durata*60)) then fasciaOraria
											when '16:00:00' <= P.Ora and P.Ora <= '20:00:00' then pom
                                            when '20:00:00' <= P.Ora and P.Ora <= '24:00:00' then ser
										end)) = 0) then
				#se e 0 elimino il turno del dipendete a quella fascia oraria
                delete 
                from TurnoDelDipendente
                where CodiceFiscaleDipendente = OLD.Dipendente and FasciaOrariaTurno = fasciaOraria and GiornoTurno = OLD.Data;
                
                #verifico che ci siano ALTRE proiezioni per quella fascia oraria, se no, elimino le maschere di quella fascia oraria
                if ((select count(*)
					from Proiezione P join Film F on P.Film = F.Nome
                    where P.Data = OLD.Data and P.Cinema = OLD.Cinema and (P.Ora, P.Dipendente) <> (OLD.Ora, OLD.Dipendente)
					and fasciaOraria = (case
											when P.Ora <= '20:00:00' and '20:00:00' <= ADDTIME(P.Ora, SEC_TO_TIME(F.Durata*60)) then fasciaOraria
											when '16:00:00' <= P.Ora and P.Ora <= '20:00:00' then pom
                                            when '20:00:00' <= P.Ora and P.Ora <= '24:00:00' then ser
										end)) = 0) then
                    #se non ho proiezioni per quella fasciaOraria, non mi serovno maschere
                    delete 
                    from TurnoDelDipendente
                    where GiornoTurno = OLD.Data and FasciaOrariaTurno = fasciaOraria and CodiceFiscaleDipendente in
								(select D.CodiceFiscale
								 from Dipendente D
								 where D.Cinema = OLD.Cinema and D.Tipo = 'Maschera');
				
				end if;
                
			end if;
            #se e maggiore di 0, non lo elimino, il dipendete ha un turno associato ad un altra proiezinoe
		
        else
			#se la proiezione che si vuole cancellare occupava al dipendente entrambi i turni,
			#verifico sia per il turno di sera che di pomeriggio che il dipendente abbia schedulate
			# altre proiezinoi, in caso contratio, gli cancello il turno (uno dei due o entrambi)
			if ((select count(*) 
				from Proiezione P join Film F on P.Film = F.Nome
                where P.Ora <> OLD.Ora and P.Data = OLD.Data and P.Dipendente = OLD.Dipendente and P.Cinema = OLD.Cinema and 
						('20:00:00' <= P.Ora and P.Ora <= '24:00:00' or 
                        P.Ora <= '20:00:00' and '20:00:00' <= ADDTIME(P.Ora, SEC_TO_TIME(F.Durata*60))  )) = 0) then
					delete 
					from TurnoDelDipendente
					where CodiceFiscaleDipendente = OLD.Dipendente and FasciaOrariaTurno = ser and GiornoTurno = OLD.Data;
                
                #analogamente a sopra
                #elimino le 2 maschere serali se oltre la proiezione da eliminare non ne ho altre nella stessa fscia oraria
                if ((select count(*) 
					from Proiezione P join Film F on P.Film = F.Nome
					where P.Data = OLD.Data and P.Cinema = OLD.Cinema and (P.Ora, P.Dipendente) <> (OLD.Ora, OLD.Dipendente) and
						('20:00:00' <= P.Ora and P.Ora <= '24:00:00' or 
                        P.Ora <= '20:00:00' and '20:00:00' <= ADDTIME(P.Ora, SEC_TO_TIME(F.Durata*60))  )) = 0) then
                    delete 
                    from TurnoDelDipendente
                    where GiornoTurno = OLD.Data and FasciaOrariaTurno = ser and CodiceFiscaleDipendente in
								(select D.CodiceFiscale
								 from Dipendente D
								 where D.Cinema = OLD.Cinema and D.Tipo = 'Maschera');
				end if;
                    
			end if;
            
			if ((select count(*) 
				from Proiezione P join Film F on P.Film = F.Nome
                where P.Ora <> OLD.Ora and P.Data = OLD.Data and P.Dipendente = OLD.Dipendente and P.Cinema = OLD.Cinema and 
						('16:00:00' <= P.Ora and P.Ora <= '20:00:00' or 
                        P.Ora <= '20:00:00' and '20:00:00' <= ADDTIME(P.Ora, SEC_TO_TIME(F.Durata*60))  )) = 0) then
					delete 
					from TurnoDelDipendente
					where CodiceFiscaleDipendente = OLD.Dipendente and FasciaOrariaTurno = pom and GiornoTurno = OLD.Data;
                    
                if ((select count(*) 
					from Proiezione P join Film F on P.Film = F.Nome
					where P.Data = OLD.Data and P.Cinema = OLD.Cinema and (P.Ora, P.Dipendente) <> (OLD.Ora, OLD.Dipendente) and
						('16:00:00' <= P.Ora and P.Ora <= '20:00:00' or 
                        P.Ora <= '20:00:00' and '20:00:00' <= ADDTIME(P.Ora, SEC_TO_TIME(F.Durata*60))  )) = 0) then
                    delete 
                    from TurnoDelDipendente
                    where GiornoTurno = OLD.Data and FasciaOrariaTurno = fasciaOraria and CodiceFiscaleDipendente in
								(select D.CodiceFiscale
								 from Dipendente D
								 where D.Cinema = OLD.Cinema and D.Tipo = 'Maschera');
				end if;
                
			end if;
        
		end if;

    end if;
    
    select F.Durata
    from Film F
    where F.Nome = NEW.Film
    into durata_p;
    
    #identifico la facia oraria in base all'ora della proiezione
    if ('16:00:00' <= NEW.Ora and NEW.Ora <= '20:00:00') then
		set fasciaOraria = pom;
	end if;
    if ('20:00:00' <= NEW.Ora and NEW.Ora <= '24:00:00') then
		set fasciaOraria = ser;
	end if;
    #se ho una proiezione che si sovrappone tra i 2 turni
    if (NEW.Ora <= '20:00:00'  and '20:00:00' <= ADDTIME(NEW.Ora, SEC_TO_TIME(durata_p*60))) then
		set entrambi = 1;
    end if;
    if ('24:00:00' < NEW.Ora or NEW.Ora < '16:00:00') then
		signal sqlstate '45009'
		set message_text = 'Ora della proiezione non valida, deve essere compresa tra le 16 e le 24';
	end if;
    
	if NEW.Dipendente is not null then
    
		if not entrambi then
			#prima verifico che sia presente nella tabella Turno la tupla:
			if ((select FasciaOraria
				from Turno T
				where T.Giorno = NEW.Data and T.Cinema = NEW.Cinema and T.FasciaOraria = fasciaOraria)
				is null) then
				#se non è presente, la creo
				insert into Turno values (fasciaOraria, NEW.Data, NEW.Cinema);
			end if;
			#se e presente devo verificare che il dipendente (se e stato selezionato per la proiezione
			#allora e sicuramente libero per come funzionano le procedure sviluppate) sia associato a quel turno
			#e se non lo fosse lo associo
			if ((select CodiceFiscaleDipendente
				from TurnoDelDipendente TD
				where TD.CodiceFiscaleDipendente = NEW.Dipendente and TD.GiornoTurno = NEW.Data and TD.FasciaOrariaTurno = fasciaOraria)
				is null )then
				insert into TurnoDelDipendente
				values (fasciaOraria, NEW.Data, NEW.Dipendente);
			end if;
            
            #faccio in modo che ci siano almeno 2 maschere differenti per la fascia oraria in cui ho la proiezione
            select count(distinct TD.CodiceFiscaleDipendente)
			from TurnoDelDipendente TD right join Dipendente D on TD.CodiceFiscaleDipendente = D.CodiceFiscale
			where TD.GiornoTurno = NEW.Data and D.Cinema = NEW.Cinema and TD.FasciaOrariaTurno = fasciaOraria and D.Tipo = 'Maschera'
            into maschereDisponibili;
            #per tante volte quante sono le maschere mancanti per arrivare a 2
            #aggiungo al turno una maschera a caso disponibile.
            #definisco una maschera disponibile per un cinema se: 
            #1)lavora per quel cinema
			#2)non ha un turno associato alla fascia oraria della proiezione
            while maschereDisponibili < 2 do
				
                select CodiceFiscale 
                from Dipendente
                where Tipo = 'Maschera' and Cinema = NEW.Cinema and CodiceFiscale not in(
					select D.CodiceFiscale
					from Dipendente D left join TurnoDelDipendente TD on D.CodiceFiscale = TD.CodiceFiscaleDipendente
							and D.Cinema = NEW.Cinema
					where TD.FasciaOrariaTurno = fasciaOraria and TD.GiornoTurno = NEW.Data and D.Tipo = 'Maschera')
				order by RAND() limit 1 into mascheraCasuale;
                
                insert into TurnoDelDipendente
                values (fasciaOraria, NEW.Data, mascheraCasuale);
                
                set maschereDisponibili = maschereDisponibili +1;
            
            end while;			
		#se il proiezionista deve gestire una proiezione tale per cui gli occupa entrambi i turni della giornata:
		else
			#devo verificare che sia presente sia il turno pomeridiano che quello serale
			if ((select FasciaOraria
				from Turno T
				where T.Giorno = NEW.Data and T.Cinema = NEW.Cinema and T.FasciaOraria = pom)
				is null) then
				insert into Turno values (pom, NEW.Data, NEW.Cinema);
			end if;
			if ((select FasciaOraria
				from Turno T
				where T.Giorno = NEW.Data and T.Cinema = NEW.Cinema and T.FasciaOraria = ser)
				is null) then
				insert into Turno values (ser, NEW.Data, NEW.Cinema);
			end if;
			#resta da verificare che il dipendente scelto per la gestione della proiezione
			#sia associato ai turni sia serale che pomeridiano
			if ((select CodiceFiscaleDipendente
				from TurnoDelDipendente TD
				where TD.CodiceFiscaleDipendente = NEW.Dipendente and TD.GiornoTurno = NEW.Data and TD.FasciaOrariaTurno = pom)
				is null )then
				insert into TurnoDelDipendente
				values (pom, NEW.Data, NEW.Dipendente);
			end if;
			if ((select CodiceFiscaleDipendente
				from TurnoDelDipendente TD
				where TD.CodiceFiscaleDipendente = NEW.Dipendente and TD.GiornoTurno = NEW.Data and TD.FasciaOrariaTurno = ser)
				is null )then
				insert into TurnoDelDipendente
				values (ser, NEW.Data, NEW.Dipendente);
			end if;
            #stessa cosa per le maschere, almeno 2 per il turno di sera e quello pomeridiano
            select count(distinct TD.CodiceFiscaleDipendente)
			from TurnoDelDipendente TD right join Dipendente D on TD.CodiceFiscaleDipendente = D.CodiceFiscale
			where TD.GiornoTurno = NEW.Data and D.Cinema = NEW.Cinema and TD.FasciaOrariaTurno = ser and D.Tipo = 'Maschera'
            into maschereDisponibiliSerali;
            
            select count(distinct TD.CodiceFiscaleDipendente)
			from TurnoDelDipendente TD right join Dipendente D on TD.CodiceFiscaleDipendente = D.CodiceFiscale
			where TD.GiornoTurno = NEW.Data and D.Cinema = NEW.Cinema and TD.FasciaOrariaTurno = pom and D.Tipo = 'Maschera'
            into maschereDisponibiliPomeridiane;
            
            while maschereDisponibiliSerali < 2 do
				
                select CodiceFiscale 
                from Dipendente
                where Tipo = 'Maschera' and Cinema = NEW.Cinema and CodiceFiscale not in(
					select D.CodiceFiscale
					from Dipendente D left join TurnoDelDipendente TD on D.CodiceFiscale = TD.CodiceFiscaleDipendente
							and D.Cinema = NEW.Cinema
					where TD.FasciaOrariaTurno = ser and TD.GiornoTurno = NEW.Data and D.Tipo = 'Maschera')
				order by RAND() limit 1 into mascheraCasuale;
                
                insert into TurnoDelDipendente
                values (ser, NEW.Data, mascheraCasuale);
                
                set maschereDisponibiliSerali = maschereDisponibiliSerali +1;
            
            end while;
            while maschereDisponibiliPomeridiane < 2 do
				
                select CodiceFiscale 
                from Dipendente
                where Tipo = 'Maschera' and Cinema = NEW.Cinema and CodiceFiscale not in(
					select D.CodiceFiscale
					from Dipendente D left join TurnoDelDipendente TD on D.CodiceFiscale = TD.CodiceFiscaleDipendente
							and D.Cinema = NEW.Cinema
					where TD.FasciaOrariaTurno = pom and TD.GiornoTurno = NEW.Data and D.Tipo = 'Maschera')
				order by RAND() limit 1 into mascheraCasuale;
                
                insert into TurnoDelDipendente
                values (pom, NEW.Data, mascheraCasuale);
                
                set maschereDisponibiliPomeridiane = maschereDisponibiliPomeridiane +1;
            
            end while;
		end if;
	end if;
    #se e stata inserita una proiezione senza specificare il dipendente
    #non faccio nulla
    
END$$


USE `CatenaCinema`$$
DROP TRIGGER IF EXISTS `CatenaCinema`.`Proiezione_BEFORE_DELETE` $$
USE `CatenaCinema`$$
CREATE DEFINER = CURRENT_USER TRIGGER `CatenaCinema`.`Proiezione_BEFORE_DELETE` BEFORE DELETE ON `Proiezione` FOR EACH ROW
BEGIN
	#prima di cancellare una proiezione devo eliminare l'associazione tra il proiezionista
    #(qualora esistesse) e la proiezione stessa, e controllare se nella data e fascia oraria 
    #non ci sono altre proiezioni devo eliminare anche le maschere che dovevano timbrare i biglietti all ingresso
    
    #se elimino qualche turno di qualche tipo di dipendente, devo eliminare anche 
    #il turno associato al cinema, se la proiezione eliminata era l'unica di quella data in quella fascia oraria
    
    declare pom varchar(10) default '16-20';
    declare ser varchar(10) default '20-24';
    declare fasciaOraria varchar(10);
    declare entrambi int default 0;
    declare durata_p int;
    
    if (OLD.Dipendente is not null) then
    
		select F.Durata
		from Film F
		where F.Nome = OLD.Film
		into durata_p;
		
		if ('16:00:00' <= OLD.Ora and OLD.Ora <= '20:00:00') then
			set fasciaOraria = pom;
		end if;
		if ('20:00:00' <= OLD.Ora and OLD.Ora <= '24:00:00') then
			set fasciaOraria = ser;
		end if;
		#se ho una proiezione che si sovrappone tra i 2 turni
		if (OLD.Ora <= '20:00:00'  and '20:00:00' <= ADDTIME(OLD.Ora, SEC_TO_TIME(durata_p*60))) then
		set entrambi = 1;
		end if;
		
        # se il proiezionista non ha associate altre proiezioni per quel giorno a quella fascia oraria
        # gli cancello il turno, o entrambi i turni, qualora la proiezione che verra occupata gli occupi
        # entrambi i turni
        if not entrambi then
        
			if ((select count(*) 
				from Proiezione P join Film F on P.Film = F.Nome
                where P.Ora <> OLD.Ora and P.Data = OLD.Data and P.Dipendente = OLD.Dipendente and P.Cinema = OLD.Cinema and 
						# calcolo la facia oraria per quel film
						fasciaOraria = (case
											# se in quel giorno il proiezionista ha un altro film che gli occupa
											# entrambi i turni, non voglio cancellarglieli
											when P.Ora <= '20:00:00' and '20:00:00' <= ADDTIME(P.Ora, SEC_TO_TIME(F.Durata*60)) then fasciaOraria
											when '16:00:00' <= P.Ora and P.Ora <= '20:00:00' then pom
                                            when '20:00:00' <= P.Ora and P.Ora <= '24:00:00' then ser
										end)) = 0) then
				#se e 0 elimino il turno del dipendete a quella fascia oraria
                delete 
                from TurnoDelDipendente
                where CodiceFiscaleDipendente = OLD.Dipendente and FasciaOrariaTurno = fasciaOraria and GiornoTurno = OLD.Data;
                
                #verifico che ci siano ALTRE proiezioni per quella fascia oraria, se no, elimino le maschere di quella fascia oraria
                if ((select count(*)
					from Proiezione P join Film F on P.Film = F.Nome
                    where P.Data = OLD.Data and P.Cinema = OLD.Cinema and (P.Ora, P.Dipendente) <> (OLD.Ora, OLD.Dipendente)
					and fasciaOraria = (case
											when P.Ora <= '20:00:00' and '20:00:00' <= ADDTIME(P.Ora, SEC_TO_TIME(F.Durata*60)) then fasciaOraria
											when '16:00:00' <= P.Ora and P.Ora <= '20:00:00' then pom
                                            when '20:00:00' <= P.Ora and P.Ora <= '24:00:00' then ser
										end)) = 0) then
                    #se non ho proiezioni per quella fasciaOraria, non mi serovno maschere
                    delete 
                    from TurnoDelDipendente
                    where GiornoTurno = OLD.Data and FasciaOrariaTurno = fasciaOraria and CodiceFiscaleDipendente in
								(select D.CodiceFiscale
								 from Dipendente D
								 where D.Cinema = OLD.Cinema and D.Tipo = 'Maschera');
				
				end if;
                
			end if;
            #se e maggiore di 0, non lo elimino, il dipendete ha un turno associato ad un altra proiezinoe
		
        else
			#se la proiezione che si vuole cancellare occupava al dipendente entrambi i turni,
			#verifico sia per il turno di sera che di pomeriggio che il dipendente abbia schedulate
			# altre proiezinoi, in caso contratio, gli cancello il turno (uno dei due o entrambi)
			if ((select count(*) 
				from Proiezione P join Film F on P.Film = F.Nome
                where P.Ora <> OLD.Ora and P.Data = OLD.Data and P.Dipendente = OLD.Dipendente and P.Cinema = OLD.Cinema and 
						('20:00:00' <= P.Ora and P.Ora <= '24:00:00' or 
                        P.Ora <= '20:00:00' and '20:00:00' <= ADDTIME(P.Ora, SEC_TO_TIME(F.Durata*60))  )) = 0) then
					delete 
					from TurnoDelDipendente
					where CodiceFiscaleDipendente = OLD.Dipendente and FasciaOrariaTurno = ser and GiornoTurno = OLD.Data;
                
                #analogamente a sopra
                #elimino le 2 maschere serali se oltre la proiezione da eliminare non ne ho altre nella stessa fscia oraria
                if ((select count(*) 
					from Proiezione P join Film F on P.Film = F.Nome
					where P.Data = OLD.Data and P.Cinema = OLD.Cinema and (P.Ora, P.Dipendente) <> (OLD.Ora, OLD.Dipendente) and
						('20:00:00' <= P.Ora and P.Ora <= '24:00:00' or 
                        P.Ora <= '20:00:00' and '20:00:00' <= ADDTIME(P.Ora, SEC_TO_TIME(F.Durata*60))  )) = 0) then
                    delete 
                    from TurnoDelDipendente
                    where GiornoTurno = OLD.Data and FasciaOrariaTurno = ser and CodiceFiscaleDipendente in
								(select D.CodiceFiscale
								 from Dipendente D
								 where D.Cinema = OLD.Cinema and D.Tipo = 'Maschera');
				end if;
                    
			end if;
            
			if ((select count(*) 
				from Proiezione P join Film F on P.Film = F.Nome
                where P.Ora <> OLD.Ora and P.Data = OLD.Data and P.Dipendente = OLD.Dipendente and P.Cinema = OLD.Cinema and 
						('16:00:00' <= P.Ora and P.Ora <= '20:00:00' or 
                        P.Ora <= '20:00:00' and '20:00:00' <= ADDTIME(P.Ora, SEC_TO_TIME(F.Durata*60))  )) = 0) then
					delete 
					from TurnoDelDipendente
					where CodiceFiscaleDipendente = OLD.Dipendente and FasciaOrariaTurno = pom and GiornoTurno = OLD.Data;
                    
                if ((select count(*) 
					from Proiezione P join Film F on P.Film = F.Nome
					where P.Data = OLD.Data and P.Cinema = OLD.Cinema and (P.Ora, P.Dipendente) <> (OLD.Ora, OLD.Dipendente) and
						('16:00:00' <= P.Ora and P.Ora <= '20:00:00' or 
                        P.Ora <= '20:00:00' and '20:00:00' <= ADDTIME(P.Ora, SEC_TO_TIME(F.Durata*60))  )) = 0) then
                    delete 
                    from TurnoDelDipendente
                    where GiornoTurno = OLD.Data and FasciaOrariaTurno = fasciaOraria and CodiceFiscaleDipendente in
								(select D.CodiceFiscale
								 from Dipendente D
								 where D.Cinema = OLD.Cinema and D.Tipo = 'Maschera');
				end if;
                
			end if;
        
		end if;

    end if;
END$$


USE `CatenaCinema`$$
DROP TRIGGER IF EXISTS `CatenaCinema`.`CartaDiCredito_BEFORE_INSERT` $$
USE `CatenaCinema`$$
CREATE DEFINER = CURRENT_USER TRIGGER `CatenaCinema`.`CartaDiCredito_BEFORE_INSERT` BEFORE INSERT ON `CartaDiCredito` FOR EACH ROW
BEGIN

	#verifico che la data sia un numero compreso tra 1 e 12,
	#che l'anno sia di 4 cifre
    #il CVV di 3 cifre
	#e il numero della carta di 16 cifre
    
    if (LENGTH(NEW.Numero) != 16) then
		signal sqlstate '45021'
		set message_text = 'Numero carta non valido';
	end if;
    
	if ( 13 <= NEW.MeseScadenza or NEW.MeseScadenza <= 0) then
		signal sqlstate '45019'
		set message_text = 'Mese di scadenza non valido';
	end if;
    
	if ( 0 >= NEW.AnnoScadenza or NEW.AnnoScadenza >= 100) then
		signal sqlstate '45022'
		set message_text = 'Anno di scadenza non valido';
	end if;
    
	if ( 0 >= NEW.CVV or NEW.CVV >= 1000) then
		signal sqlstate '45020'
		set message_text = 'CVV non valido';
	end if;

END$$


USE `CatenaCinema`$$
DROP TRIGGER IF EXISTS `CatenaCinema`.`CartaDiCredito_BEFORE_UPDATE` $$
USE `CatenaCinema`$$
CREATE DEFINER = CURRENT_USER TRIGGER `CatenaCinema`.`CartaDiCredito_BEFORE_UPDATE` BEFORE UPDATE ON `CartaDiCredito` FOR EACH ROW
BEGIN
	#verifico che la data sia un numero compreso tra 1 e 12,
	#che l'anno sia di 4 cifre
    #il CVV di 3 cifre
	#e il numero della carta di 16 cifre
    
    if (LENGTH(NEW.Numero) != 16) then
		signal sqlstate '45021'
		set message_text = 'Numero carta non valido';
	end if;
    
	if ( 13 <= NEW.MeseScadenza or NEW.MeseScadenza <= 0) then
		signal sqlstate '45019'
		set message_text = 'Mese di scadenza non valido';
	end if;
    
	if ( 0 >= NEW.AnnoScadenza or NEW.AnnoScadenza >= 10000) then
		signal sqlstate '45022'
		set message_text = 'Anno di scadenza non valido';
	end if;
    
	if ( 0 >= NEW.CVV or NEW.CVV >= 1000) then
		signal sqlstate '45020'
		set message_text = 'CVV non valido';
	end if;
END$$


USE `CatenaCinema`$$
DROP TRIGGER IF EXISTS `CatenaCinema`.`Biglietto_AFTER_INSERT` $$
USE `CatenaCinema`$$
CREATE DEFINER = CURRENT_USER TRIGGER `CatenaCinema`.`Biglietto_AFTER_INSERT` AFTER INSERT ON `Biglietto` FOR EACH ROW
BEGIN
	#dopo che è stato inserito un biglietto
    #aggiorno l'incasso totale della relativa proiezione
    
    declare costo int;
    declare incasso int;
    
    select P.Costo, P.IncassoTotale
    from Proiezione P
	where P.Cinema = NEW.CinemaDellaProiezione and P.Sala = NEW.SalaDellaProiezione and P.Ora = NEW.OraProiezione
			and P.Film = NEW.Film and P.Data = NEW.DataProiezione
	into costo, incasso;
    
    if incasso is null then
		set incasso = 0;
	end if;
    
    set incasso = incasso + costo;
    
    update Proiezione 
    set IncassoTotale = incasso
    where Cinema = NEW.CinemaDellaProiezione and Sala = NEW.SalaDellaProiezione and Ora = NEW.OraProiezione
			and Film = NEW.Film and Data = NEW.DataProiezione;

END$$


USE `CatenaCinema`$$
DROP TRIGGER IF EXISTS `CatenaCinema`.`Biglietto_AFTER_UPDATE` $$
USE `CatenaCinema`$$
CREATE DEFINER = CURRENT_USER TRIGGER `CatenaCinema`.`Biglietto_AFTER_UPDATE` AFTER UPDATE ON `Biglietto` FOR EACH ROW
BEGIN
	#se e' stato aggiornato il suo tipo in 'Annullato' devo decrementare il costo dall'incasso totale
    declare costo int;
    declare incasso int;
    
    if(NEW.Tipo = 'Annullato') then 
		select P.Costo, P.IncassoTotale
		from Proiezione P
		where P.Cinema = NEW.CinemaDellaProiezione and P.Sala = NEW.SalaDellaProiezione and P.Ora = NEW.OraProiezione
				and P.Film = NEW.Film and P.Data = NEW.DataProiezione
		into costo, incasso;
		
		set incasso = incasso - costo;
        
		update Proiezione 
        set IncassoTotale = incasso 
        where Cinema = NEW.CinemaDellaProiezione and Sala = NEW.SalaDellaProiezione and Ora = NEW.OraProiezione
				and Film = NEW.Film and Data = NEW.DataProiezione;
    end if;
END$$


DELIMITER ;

delimiter !
create function `cartaEsistente`(numero varchar(16))
returns bool
begin
    if ((select count(*) from CartaDiCredito C where C.Numero = numero) > 0) then
		return true;
    end if;
    return false;
end!
delimiter ;

call popolaSalePosti('1');
source dipendentiCatenaDump.dump;
source attoriDump.dump;
source filmDump.dump;
call popolaAttoriNelFilm();
call popolaProiezione('1', '1', '4');
call popolaBiglietti('1', '1', '6');
