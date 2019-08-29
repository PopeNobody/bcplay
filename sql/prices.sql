CREATE TABLE prices (
	date datetime NOT NULL,
  sym varchar(10) NOT NULL,
  price decimal(20,10) default null,
  primary key (date, sym)
);
