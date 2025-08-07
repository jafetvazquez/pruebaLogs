-- Table: public.logmsgs

-- DROP TABLE IF EXISTS public.logmsgs;

CREATE TABLE IF NOT EXISTS public.logmsgs
(
    id integer NOT NULL DEFAULT nextval('logmsgs_id_seq'::regclass),
    mensaje text COLLATE pg_catalog."default" NOT NULL,
    dia smallint NOT NULL,
    mes smallint NOT NULL,
    anio integer NOT NULL,
    tienda integer NOT NULL,
    "timestamp" timestamp with time zone DEFAULT CURRENT_TIMESTAMP,
    CONSTRAINT logmsgs_pkey PRIMARY KEY (id)
)

TABLESPACE pg_default;

ALTER TABLE IF EXISTS public.logmsgs
    OWNER to postgres;