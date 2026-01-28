|*******************************************************************************
|* txsli4200m000  B61O_a_ext
|* Invoicing Position
|* 45118
|* 2021-10-28 [10:52]
|*******************************************************************************
|* Script Type: 123
|*******************************************************************************
|* ELT_SLI_001, Clauber Correia, 28/10/2021
|* Create session
|*
|*************************** declaration section *******************************
declaration:

	table	tbtsli200 |* Faturamento
	table	tbtsli201
	table	tbtsli202
	table	tbtsli203
	table	tlpbra241
	table	tlpbra240
	table	tbtftb002
	table	tbtcom100
	table	tbtcom130
	table	ttcmcs003

	extern	domain	btyesno		fbob.txt
	extern	domain	btorno		frm.fire.f	fixed
	extern	domain	btorno		frm.fire.t	fixed
	extern	domain	btdate		frm.date.f
	extern	domain	btdate		frm.date.t
	extern	domain	btcfoc		frm.ccfo.f	|cfop
	extern	domain	btcfoc		frm.ccfo.t	|cfop
	extern	domain	btmcs.fdtc	frm.fdtc.f	|tipo documento
	extern	domain	btmcs.fdtc	frm.fdtc.t	|tipo documento
	extern	domain	btcom.bpid	frm.bpid.f
	extern	domain	btcom.bpid	frm.bpid.t
	extern	domain	btsli.stat	frm.stat.f
	extern	domain	btsli.stat	frm.stat.t
	extern	domain	btmcs.cste	frm.cste.f
	extern	domain	btmcs.cste	frm.cste.t
	extern	domain	tccprj		frm.cprj.f
	extern	domain	tccprj		frm.cprj.t
	extern	domain	btamnt		rpt.icms
	extern	domain	btamnt		rpt.ipi
	extern	domain	tcdsca		rpt.cwar.d
	extern	domain	tcnama		contribuinte
	
	extern	domain	btamnt		l.juros
	extern	domain	btamnt		l.desc
	extern	domain	btamnt		bc.icms
	extern	domain	btamnt		bc.icst
	extern	domain	btamnt		bc.pis
	extern	domain	btamnt		bc.cofins
	extern	domain	btamnt		bc.ipi
	extern	domain	btamnt		vl.icms
	extern	domain	btamnt		vl.icst
	extern	domain	btamnt		vl.pis
	extern	domain	btamnt		vl.cofins
	extern	domain	btamnt		vl.ipi
	extern	domain	btamnt		vl.desconto
			long		ano
			long		mes
			long		dia
			long 		dummy	
	
			long		l.ret
	
			string		registro(900)
			string		status(20)
			string		status.nfe(20)
			string		data.emis(20)
			string		data.fisc(20)
			long		arquivo
		domain	tcmcs.str215m	g.tmp.file.path	|Path BSE
			
	
	#include <bic_desktop>

|****************************** program section ********************************


|****************************** group section **********************************

group.1:
init.group:
	get.screen.defaults()

|****************************** choice section ********************************

choice.cont.process:
on.choice:
	execute(print.data)

choice.print.data:
on.choice:
|	if rprt_open() then
		read.main.table()
|		rprt_close()
|	else
|		choice.again()
|	endif


|****************************** field section *********************************

field.frm.fire.f:
when.field.changes:
	frm.fire.t = frm.fire.f
	
field.frm.date.f:
when.field.changes:
	frm.date.t = frm.date.f
	
field.frm.ccfo.f:
when.field.changes:
	frm.ccfo.t = frm.ccfo.f
	
field.frm.fdtc.f:
when.field.changes:
	frm.fdtc.t = frm.fdtc.f
	
field.frm.bpid.f:
when.field.changes:
	frm.bpid.t = frm.bpid.f
	
field.frm.stat.f:
when.field.changes:
	frm.stat.t = frm.stat.f


|****************************** function section ******************************

functions:

function read.main.table()
{
|	handle.file.selection()
|	arquivo = seq.open(g.tmp.file.path, "w")
	g.tmp.file.path = "${BSE}/appdata/relatorio/saidas.csv"
	arquivo = seq.open(g.tmp.file.path, "w")
	
	if arquivo < 0 then
		return
	else
		registro = "Referencia;Nota Fiscal; serie; COD.PN; Cliente;CNPJ; Status; Status NFE; Origem; Fatura;"
		registro = registro&"Data Geracao;Data Fiscal;CFOP;Grupo Contabil;Tipo NF;"
		registro = registro&"Base ICMS; Valor ICMS; Base IPI; Valor IPI; Base PIS;"
		registro = registro&"Valor PIS; Base Cofins; Valor Cofins; Base ICMS ST;Valor ICMS ST;"
		registro = registro&"Valor Mercadoria; Seguro; Frete; Outras Despesas;Juros;desconto;Valor Total;"
		registro = registro&"Inscricao Estadual; Suframa; Chave Acesso; Contribuinte ICMS"
		seq.puts(registro, arquivo)
		
		select	btsli200.*
		from	btsli200
		where	btsli200._index1 inrange {:frm.fire.f}
			                     and {:frm.fire.t}
		and	btsli200.date between :frm.date.f and :frm.date.t
		and	btsli200.ccfo between :frm.ccfo.f and :frm.ccfo.t
		and	btsli200.fdtc between :frm.fdtc.f and :frm.fdtc.t
		and	btsli200.bpid between :frm.bpid.f and :frm.bpid.t
		and	btsli200.stat between :frm.stat.f and :frm.stat.t	
		order by btsli200._index1
		selectdo
			clear.variables()
			
			status = enum.descr$("btsli.stat", btsli200.stat)
			status.nfe = enum.descr$("btsli.nfes", btsli200.nfes)
			
			l.ret = utc.to.date(btsli200.datg, ano, mes, dia, dummy, dummy, dummy)
			data.emis = str$(dia)&"/"&str$(mes)&"/"&str$(ano)
			if dia < 10 then
				data.emis = "0"&str$(dia)&"/"&str$(mes)&"/"&str$(ano)
				if mes < 10 then
					data.emis = str$(dia)&"/"&"0"&str$(mes)&"/"&str$(ano)
				endif
			else
				if mes < 10 then
					data.emis = str$(dia)&"/"&"0"&str$(mes)&"/"&str$(ano)
				endif
			endif
			
			if btsli200.dats > 0 then
				l.ret = utc.to.date(btsli200.dats, ano, mes, dia, dummy, dummy, dummy)
				data.fisc = str$(dia)&"/"&str$(mes)&"/"&str$(ano)
				if dia < 10 then
					data.fisc = "0"&str$(dia)&"/"&str$(mes)&"/"&str$(ano)
					if mes < 10 then
						data.fisc = str$(dia)&"/"&"0"&str$(mes)&"/"&str$(ano)
					endif
				else
					if mes < 10 then
						data.fisc = str$(dia)&"/"&"0"&str$(mes)&"/"&str$(ano)
					endif
				endif
			endif
			
			l.ret = validate_bipd() 
			
			contribuinte = ""
			db.set.to.default(tbtftb002)
			select	btftb002.*
			from	btftb002
			where	btftb002._index1 = {:btcom130.ftyp, :btcom130.fovn}
			selectdo
			endselect
			contribuinte = enum.descr$("btyesno", btftb002.cicm)
			
			if l.ret <> 0 then
				continue
			endif				
			
			
						
			select	btsli202.*
			from	btsli202
			where	btsli202._index1 = {:btsli200.fire}
			selectdo
				on case btsli202.brty
					case btmcs.brty.icms:
						bc.icms = btsli202.base
						vl.icms = btsli202.amnt
						rpt.icms = btsli202.amnt
						break
					case btmcs.brty.ipi:
						bc.ipi = btsli202.base
						vl.ipi = btsli202.amnt
						rpt.icms = btsli202.amnt
						rpt.ipi = btsli202.amnt
						break
					case btmcs.brty.cofins:
						bc.cofins = btsli202.base
						vl.cofins = btsli202.amnt
						break
					case btmcs.brty.pis:
						bc.pis = btsli202.base
						vl.pis = btsli202.amnt
						break
					case btmcs.brty.icms.st:
						bc.icst = btsli202.base
						vl.icst = btsli202.amnt
						break
					default:
				endcase
			endselect
			
			select	lpbra240.idoc
			from	lpbra240
			where	lpbra240._index1 = {:btsli200.orno}
			selectdo
			endselect		
			
			select	lpbra241.oorn
			from	lpbra241
			where	lpbra241._index1 = {:btsli200.orno}
			as set with 1 rows
			selectdo
			endselect	
			
			select	SUM(btsli201.tldm):vl.desconto
			from	btsli201
			where	btsli201._index1 = {:btsli200.fire}
			selectdo
			endselect	
			
			select	btsli201.*
			from	btsli201
			where	btsli201._index1 = {:btsli200.fire}
			and	btsli201.cprj between :frm.cprj.f and :frm.cprj.t
			as set with 1 rows
			selectdo
				rpt.cwar.d = ""
				l.juros = 0
				l.desc = 0
				
				select	tcmcs003.*
				from	tcmcs003
				where	tcmcs003._index1 = {:btsli201.cwar}
				selectdo
					rpt.cwar.d = tcmcs003.dsca	
				endselect	
				
				if double.cmp(vl.desconto, 0.00, 0.001) < 0 then
					l.desc = abs(vl.desconto)
				endif
				if double.cmp(vl.desconto, 0.00, 0.001) > 0 then
					l.juros = vl.desconto
				endif	
			
				insert.registro()
				
			endselect		
		endselect
		
		seq.close(arquivo)
		l.ret = client.download.file(g.tmp.file.path)
		
		if not isspace(g.tmp.file.path) then
			l.ret = file.rm(g.tmp.file.path)
		endif
	endif	
}

function insert.registro()
{
	registro = btsli200.fire&";"&str$(btsli200.docn)&";"&trim$(btsli200.seri)&";"&trim$(btsli200.bpid)&";"&trim$(btcom100.nama)&";"&trim$(btcom130.fovn)&";"&trim$(status)
	registro = registro&";"&trim$(status.nfe)&";"&trim$(lpbra241.oorn)&";"&str$(lpbra240.idoc)
	registro = registro&";"&trim$(data.emis)&";"&trim$(data.fisc)&";"&btsli201.ccfo
	registro = registro&";"&trim$(btsli201.accd)&";"&trim$(btsli200.fdtc)
	registro = registro&";"&edit$(bc.icms,"ZZZZZZZZZZZZZZVD99")&";"&edit$(vl.icms,"ZZZZZZZZZZZZZZVD99")
	registro = registro&";"&edit$(bc.ipi,"ZZZZZZZZZZZZZZVD99")&";"&edit$(vl.ipi,"ZZZZZZZZZZZZZZVD99")
	registro = registro&";"&edit$(bc.pis,"ZZZZZZZZZVD99")&";"&edit$(vl.pis,"ZZZZZZZZZVD99")
	registro = registro&";"&edit$(bc.cofins,"ZZZZZZZZZZZZZZVD99")&";"&edit$(vl.cofins,"ZZZZZZZZZZZZZZVD99")
	registro = registro&";"&edit$(bc.icst,"ZZZZZZZZZZZZZZVD99")&";"&edit$(vl.icst,"ZZZZZZZZZZZZZZVD99")
	registro = registro&";"&edit$(btsli200.gamt,"ZZZZZZZZZZZZZZVD99")&";"&edit$(btsli200.insr,"ZZZZZZZZZZZZZZVD99")
	registro = registro&";"&edit$(btsli200.fght,"ZZZZZZZZZZZZZZVD99")&";"&edit$(btsli200.gexp,"ZZZZZZZZZZZZZZVD99")
	registro = registro&";"&edit$(l.juros,"ZZZZZZZZZZZZZZVD99")&";"&edit$(l.desc,"ZZZZZZZZZZZZZZVD99")&";"&edit$(btsli200.amnt,"ZZZZZZZZZZZZZZVD99")
	registro = registro&";"&trim$(btftb002.stin)&";"&trim$(btftb002.insu)&";"&trim$(btsli200.cnfe)&";"&trim$(contribuinte)
				
	seq.puts(registro, arquivo)
	
	
}

function extern long validate_bipd()
{
	select	btcom100.*
	from	btcom100
	where	btcom100._index1 = {:btsli200.bpid}
	selectdo
		select	btcom130.*
		from	btcom130
		where	btcom130._index1 = {:btcom100.cadr}
		and	btcom130.cste between :frm.cste.f and :frm.cste.t
		selectdo
			return(0)
		selectempty
			return(1)
		endselect
	endselect
	return(1)
}


function extern void handle.file.selection()
{
		
	l.ret = 0
	
	if not isspace(g.tmp.file.path) then
		l.ret = file.rm(g.tmp.file.path)
	endif
	
	|g.tmp.file.path = creat.tmp.file$(bse.tmp.dir$())

}

function clear.variables()
{
	lpbra241.oorn = ""
	lpbra240.idoc = 0
	btsli201.accd = ""
	rpt.icms = 0
	rpt.ipi = 0
	bc.icms = 0
	vl.icms = 0
	bc.ipi = 0
	vl.ipi = 0
	bc.icst = 0
	vl.icst = 0
	bc.cofins = 0
	vl.cofins = 0
	bc.pis = 0
	vl.pis = 0	
	data.emis = ""
	data.fisc = ""
	l.ret = 0
	vl.desconto = 0
}
