create or replace view price_wide
as 
select date,
	 max(btc) as btc,
	 max(bcc) as bcc,
	 max(dash) as dash,
	 max(eth) as eth,
	 max(ltc) as ltc,
	 max(pot) as pot,
	 max(thc) as thc,
	 max(xrp)as xrp,
	 max(usd) as usd
from price_raw_wide group by date

