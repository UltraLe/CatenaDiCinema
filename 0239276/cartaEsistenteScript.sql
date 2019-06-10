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