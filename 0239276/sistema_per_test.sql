--SET @@GLOBAL.sql_mode='ALLOW_INVALID_DATES';
call popolaSalePosti('0');
source dipendenteDump.dump;
source attoriDump.dump;
source filmDump.dump;
call popolaAttoriNelFilm();
call popolaProiezione('1', '0', '4');
call popolaBiglietti('1', '0', '6');