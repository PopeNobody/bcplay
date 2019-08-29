
create or replace view price_raw_wide
as
(select `prices`.`date` AS `date`,
   NULL AS `btc`,
   NULL AS `bcc`,
   NULL AS `dash`,
   NULL AS `eth`,
   NULL AS `ltc`,
   NULL AS `pot`,
   NULL AS `thc`,
   NULL AS `xrp`,
   `prices`.`price` AS `usd` from `prices`
where
(`prices`.`sym` = 'USDT')
group by
`prices`.`date`,
`prices`.`sym`)
union
(select `prices`.`date` AS `date`,
   NULL AS `btc`,
`prices`.`price` AS `bcc`,
   NULL AS `dash`,
   NULL AS `eth`,
   NULL AS `ltc`,
   NULL AS `pot`,
   NULL AS `thc`,
   NULL AS `xrp`,
   NULL AS `NULL` from `prices`
where
(`prices`.`sym` = 'BCC')
group by
`prices`.`date`,
`prices`.`sym`)
union
(select `prices`.`date` AS `date`,
`prices`.`price` AS `btc`,
   NULL AS `bcc`,
   NULL AS `dash`,
   NULL AS `eth`,
   NULL AS `ltc`,
   NULL AS `pot`,
   NULL AS `thc`,
   NULL AS `xrp`,
   NULL AS `NULL` from `prices`
where
(`prices`.`sym` = 'BTC')
group by
`prices`.`date`,
`prices`.`sym`)
union
(select `prices`.`date` AS `date`,
   NULL AS `btc`,
   NULL AS `bcc`,
`prices`.`price` AS `dash`,
   NULL AS `eth`,
   NULL AS `ltc`,
   NULL AS `pot`,
   NULL AS `thc`,
   NULL AS `xrp`,
   NULL AS `NULL` from `prices`
where
(`prices`.`sym` = 'DASH')
group by
`prices`.`date`,
`prices`.`sym`)
union
(select `prices`.`date` AS `date`,
   NULL AS `btc`,
   NULL AS `bcc`,
   NULL AS `dash`,
`prices`.`price` AS `eth`,
   NULL AS `ltc`,
   NULL AS `pot`,
   NULL AS `thc`,
   NULL AS `xrp`,
   NULL AS `NULL` from `prices`
where
(`prices`.`sym` = 'ETH')
group by
`prices`.`date`,
`prices`.`sym`)
union
(select `prices`.`date` AS `date`,
   NULL AS `btc`,
   NULL AS `bcc`,
   NULL AS `dash`,
   NULL AS `eth`,
`prices`.`price` AS `ltc`,
   NULL AS `pot`,
   NULL AS `thc`,
   NULL AS `xrp`,
   NULL AS `NULL` from `prices`
where
(`prices`.`sym` = 'LTC')
group by
`prices`.`date`,
`prices`.`sym`)
union
(select `prices`.`date` AS `date`,
   NULL AS `btc`,
   NULL AS `bcc`,
   NULL AS `dash`,
   NULL AS `eth`,
   NULL AS `ltc`,
`prices`.`price` AS `pot`,
   NULL AS `thc`,
   NULL AS `xrp`,
   NULL AS `NULL` from `prices`
where
(`prices`.`sym` = 'POT')
group by
`prices`.`date`,
`prices`.`sym`)
union
(select `prices`.`date` AS `date`,
   NULL AS `btc`,
   NULL AS `bcc`,
   NULL AS `dash`,
   NULL AS `eth`,
   NULL AS `ltc`,
   NULL AS `pot`,
`prices`.`price` AS `thc`,
   NULL AS `xrp`,
   NULL AS `NULL` from `prices`
where
(`prices`.`sym` = 'THC')
group by
`prices`.`date`,
`prices`.`sym`)
union
(select `prices`.`date` AS `date`,
   NULL AS `btc`,
   NULL AS `bcc`,
   NULL AS `dash`,
   NULL AS `eth`,
   NULL AS `ltc`,
   NULL AS `pot`,
   NULL AS `thc`,
`prices`.`price` AS `xrp`,
   NULL AS `NULL` 
from `prices`
where
(`prices`.`sym` = 'XRP')
group by
`prices`.`date`, 
`prices`.`sym`
)
