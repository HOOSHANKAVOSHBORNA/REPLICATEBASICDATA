--
-- PostgreSQL database dump
--

-- Dumped from database version 9.4.4
-- Dumped by pg_dump version 9.4.4
-- Started on 2022-05-14 17:24:31 +0430

SET statement_timeout = 0;
SET lock_timeout = 0;
SET client_encoding = 'UTF8';
SET standard_conforming_strings = on;
SET check_function_bodies = false;
SET client_min_messages = warning;

--
-- TOC entry 188 (class 3079 OID 12777)
-- Name: plpgsql; Type: EXTENSION; Schema: -; Owner: 
--

CREATE EXTENSION IF NOT EXISTS plpgsql WITH SCHEMA pg_catalog;


--
-- TOC entry 3008 (class 0 OID 0)
-- Dependencies: 188
-- Name: EXTENSION plpgsql; Type: COMMENT; Schema: -; Owner: 
--

COMMENT ON EXTENSION plpgsql IS 'PL/pgSQL procedural language';


SET search_path = public, pg_catalog;

--
-- TOC entry 201 (class 1255 OID 31095)
-- Name: clear(); Type: FUNCTION; Schema: public; Owner: postgres
--

CREATE FUNCTION clear() RETURNS boolean
    LANGUAGE plpgsql
    AS $$
BEGIN
    truncate request restart identity cascade;
    truncate person restart identity cascade;
    truncate address restart identity cascade;
    return true;
END;
$$;


ALTER FUNCTION public.clear() OWNER TO postgres;

SET default_tablespace = '';

SET default_with_oids = false;

--
-- TOC entry 185 (class 1259 OID 22761)
-- Name: ack_status; Type: TABLE; Schema: public; Owner: postgres; Tablespace: 
--

CREATE TABLE ack_status (
    id bigint NOT NULL,
    name character varying(32)
);


ALTER TABLE ack_status OWNER TO postgres;

--
-- TOC entry 184 (class 1259 OID 22759)
-- Name: ack_status_id_seq; Type: SEQUENCE; Schema: public; Owner: postgres
--

CREATE SEQUENCE ack_status_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


ALTER TABLE ack_status_id_seq OWNER TO postgres;

--
-- TOC entry 3009 (class 0 OID 0)
-- Dependencies: 184
-- Name: ack_status_id_seq; Type: SEQUENCE OWNED BY; Schema: public; Owner: postgres
--

ALTER SEQUENCE ack_status_id_seq OWNED BY ack_status.id;


--
-- TOC entry 174 (class 1259 OID 22714)
-- Name: acknowledgement_id_seq; Type: SEQUENCE; Schema: public; Owner: postgres
--

CREATE SEQUENCE acknowledgement_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


ALTER TABLE acknowledgement_id_seq OWNER TO postgres;

--
-- TOC entry 175 (class 1259 OID 22716)
-- Name: acknowledgment; Type: TABLE; Schema: public; Owner: postgres; Tablespace: 
--

CREATE TABLE acknowledgment (
    id bigint DEFAULT nextval('acknowledgement_id_seq'::regclass) NOT NULL,
    request_id bigint,
    receiver integer,
    status smallint
);


ALTER TABLE acknowledgment OWNER TO postgres;

--
-- TOC entry 187 (class 1259 OID 22770)
-- Name: address; Type: TABLE; Schema: public; Owner: postgres; Tablespace: 
--

CREATE TABLE address (
    id bigint NOT NULL,
    person_id bigint,
    address text,
    created_at bigint
);


ALTER TABLE address OWNER TO postgres;

--
-- TOC entry 186 (class 1259 OID 22768)
-- Name: address_id_seq; Type: SEQUENCE; Schema: public; Owner: postgres
--

CREATE SEQUENCE address_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


ALTER TABLE address_id_seq OWNER TO postgres;

--
-- TOC entry 3010 (class 0 OID 0)
-- Dependencies: 186
-- Name: address_id_seq; Type: SEQUENCE OWNED BY; Schema: public; Owner: postgres
--

ALTER SEQUENCE address_id_seq OWNED BY address.id;


--
-- TOC entry 183 (class 1259 OID 22753)
-- Name: person; Type: TABLE; Schema: public; Owner: postgres; Tablespace: 
--

CREATE TABLE person (
    id bigint NOT NULL,
    num integer,
    name character varying(128),
    family character varying(128),
    created_at bigint
);


ALTER TABLE person OWNER TO postgres;

--
-- TOC entry 182 (class 1259 OID 22751)
-- Name: person_id_seq; Type: SEQUENCE; Schema: public; Owner: postgres
--

CREATE SEQUENCE person_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


ALTER TABLE person_id_seq OWNER TO postgres;

--
-- TOC entry 3011 (class 0 OID 0)
-- Dependencies: 182
-- Name: person_id_seq; Type: SEQUENCE OWNED BY; Schema: public; Owner: postgres
--

ALTER SEQUENCE person_id_seq OWNED BY person.id;


--
-- TOC entry 172 (class 1259 OID 22703)
-- Name: request_id_seq; Type: SEQUENCE; Schema: public; Owner: postgres
--

CREATE SEQUENCE request_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


ALTER TABLE request_id_seq OWNER TO postgres;

--
-- TOC entry 173 (class 1259 OID 22705)
-- Name: request; Type: TABLE; Schema: public; Owner: postgres; Tablespace: 
--

CREATE TABLE request (
    id bigint DEFAULT nextval('request_id_seq'::regclass) NOT NULL,
    table_id bigint,
    table_name integer NOT NULL,
    applicant integer,
    reviewer integer,
    type integer NOT NULL,
    status integer NOT NULL,
    data bytea,
    description json,
    created_at bigint
);


ALTER TABLE request OWNER TO postgres;

--
-- TOC entry 179 (class 1259 OID 22737)
-- Name: request_status; Type: TABLE; Schema: public; Owner: postgres; Tablespace: 
--

CREATE TABLE request_status (
    id bigint NOT NULL,
    name character varying(32)
);


ALTER TABLE request_status OWNER TO postgres;

--
-- TOC entry 178 (class 1259 OID 22735)
-- Name: request_status_id_seq; Type: SEQUENCE; Schema: public; Owner: postgres
--

CREATE SEQUENCE request_status_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


ALTER TABLE request_status_id_seq OWNER TO postgres;

--
-- TOC entry 3012 (class 0 OID 0)
-- Dependencies: 178
-- Name: request_status_id_seq; Type: SEQUENCE OWNED BY; Schema: public; Owner: postgres
--

ALTER SEQUENCE request_status_id_seq OWNED BY request_status.id;


--
-- TOC entry 177 (class 1259 OID 22729)
-- Name: request_type; Type: TABLE; Schema: public; Owner: postgres; Tablespace: 
--

CREATE TABLE request_type (
    id bigint NOT NULL,
    name character varying(32)
);


ALTER TABLE request_type OWNER TO postgres;

--
-- TOC entry 181 (class 1259 OID 22745)
-- Name: table_name; Type: TABLE; Schema: public; Owner: postgres; Tablespace: 
--

CREATE TABLE table_name (
    id bigint NOT NULL,
    name character varying(32)
);


ALTER TABLE table_name OWNER TO postgres;

--
-- TOC entry 180 (class 1259 OID 22743)
-- Name: table_name_id_seq; Type: SEQUENCE; Schema: public; Owner: postgres
--

CREATE SEQUENCE table_name_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


ALTER TABLE table_name_id_seq OWNER TO postgres;

--
-- TOC entry 3013 (class 0 OID 0)
-- Dependencies: 180
-- Name: table_name_id_seq; Type: SEQUENCE OWNED BY; Schema: public; Owner: postgres
--

ALTER SEQUENCE table_name_id_seq OWNED BY table_name.id;


--
-- TOC entry 176 (class 1259 OID 22727)
-- Name: type_id_seq; Type: SEQUENCE; Schema: public; Owner: postgres
--

CREATE SEQUENCE type_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


ALTER TABLE type_id_seq OWNER TO postgres;

--
-- TOC entry 3014 (class 0 OID 0)
-- Dependencies: 176
-- Name: type_id_seq; Type: SEQUENCE OWNED BY; Schema: public; Owner: postgres
--

ALTER SEQUENCE type_id_seq OWNED BY request_type.id;


--
-- TOC entry 2852 (class 2604 OID 22764)
-- Name: id; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY ack_status ALTER COLUMN id SET DEFAULT nextval('ack_status_id_seq'::regclass);


--
-- TOC entry 2853 (class 2604 OID 22773)
-- Name: id; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY address ALTER COLUMN id SET DEFAULT nextval('address_id_seq'::regclass);


--
-- TOC entry 2851 (class 2604 OID 22756)
-- Name: id; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY person ALTER COLUMN id SET DEFAULT nextval('person_id_seq'::regclass);


--
-- TOC entry 2849 (class 2604 OID 22740)
-- Name: id; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY request_status ALTER COLUMN id SET DEFAULT nextval('request_status_id_seq'::regclass);


--
-- TOC entry 2848 (class 2604 OID 22732)
-- Name: id; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY request_type ALTER COLUMN id SET DEFAULT nextval('type_id_seq'::regclass);


--
-- TOC entry 2850 (class 2604 OID 22748)
-- Name: id; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY table_name ALTER COLUMN id SET DEFAULT nextval('table_name_id_seq'::regclass);


--
-- TOC entry 2998 (class 0 OID 22761)
-- Dependencies: 185
-- Data for Name: ack_status; Type: TABLE DATA; Schema: public; Owner: postgres
--

COPY ack_status (id, name) FROM stdin;
1	pending
2	sended
\.


--
-- TOC entry 3015 (class 0 OID 0)
-- Dependencies: 184
-- Name: ack_status_id_seq; Type: SEQUENCE SET; Schema: public; Owner: postgres
--

SELECT pg_catalog.setval('ack_status_id_seq', 1, false);


--
-- TOC entry 3016 (class 0 OID 0)
-- Dependencies: 174
-- Name: acknowledgement_id_seq; Type: SEQUENCE SET; Schema: public; Owner: postgres
--

SELECT pg_catalog.setval('acknowledgement_id_seq', 5, true);


--
-- TOC entry 2988 (class 0 OID 22716)
-- Dependencies: 175
-- Data for Name: acknowledgment; Type: TABLE DATA; Schema: public; Owner: postgres
--

COPY acknowledgment (id, request_id, receiver, status) FROM stdin;
\.


--
-- TOC entry 3000 (class 0 OID 22770)
-- Dependencies: 187
-- Data for Name: address; Type: TABLE DATA; Schema: public; Owner: postgres
--

COPY address (id, person_id, address, created_at) FROM stdin;
\.


--
-- TOC entry 3017 (class 0 OID 0)
-- Dependencies: 186
-- Name: address_id_seq; Type: SEQUENCE SET; Schema: public; Owner: postgres
--

SELECT pg_catalog.setval('address_id_seq', 1, false);


--
-- TOC entry 2996 (class 0 OID 22753)
-- Dependencies: 183
-- Data for Name: person; Type: TABLE DATA; Schema: public; Owner: postgres
--

COPY person (id, num, name, family, created_at) FROM stdin;
\.


--
-- TOC entry 3018 (class 0 OID 0)
-- Dependencies: 182
-- Name: person_id_seq; Type: SEQUENCE SET; Schema: public; Owner: postgres
--

SELECT pg_catalog.setval('person_id_seq', 1, false);


--
-- TOC entry 2986 (class 0 OID 22705)
-- Dependencies: 173
-- Data for Name: request; Type: TABLE DATA; Schema: public; Owner: postgres
--

COPY request (id, table_id, table_name, applicant, reviewer, type, status, data, description, created_at) FROM stdin;
\.


--
-- TOC entry 3019 (class 0 OID 0)
-- Dependencies: 172
-- Name: request_id_seq; Type: SEQUENCE SET; Schema: public; Owner: postgres
--

SELECT pg_catalog.setval('request_id_seq', 95, true);


--
-- TOC entry 2992 (class 0 OID 22737)
-- Dependencies: 179
-- Data for Name: request_status; Type: TABLE DATA; Schema: public; Owner: postgres
--

COPY request_status (id, name) FROM stdin;
1	checking
2	waiting
4	accepted
8	edited
16	rejected
\.


--
-- TOC entry 3020 (class 0 OID 0)
-- Dependencies: 178
-- Name: request_status_id_seq; Type: SEQUENCE SET; Schema: public; Owner: postgres
--

SELECT pg_catalog.setval('request_status_id_seq', 1, false);


--
-- TOC entry 2990 (class 0 OID 22729)
-- Dependencies: 177
-- Data for Name: request_type; Type: TABLE DATA; Schema: public; Owner: postgres
--

COPY request_type (id, name) FROM stdin;
1	insert
2	update
4	delete
\.


--
-- TOC entry 2994 (class 0 OID 22745)
-- Dependencies: 181
-- Data for Name: table_name; Type: TABLE DATA; Schema: public; Owner: postgres
--

COPY table_name (id, name) FROM stdin;
1	person
2	address
\.


--
-- TOC entry 3021 (class 0 OID 0)
-- Dependencies: 180
-- Name: table_name_id_seq; Type: SEQUENCE SET; Schema: public; Owner: postgres
--

SELECT pg_catalog.setval('table_name_id_seq', 1, false);


--
-- TOC entry 3022 (class 0 OID 0)
-- Dependencies: 176
-- Name: type_id_seq; Type: SEQUENCE SET; Schema: public; Owner: postgres
--

SELECT pg_catalog.setval('type_id_seq', 1, false);


--
-- TOC entry 2869 (class 2606 OID 22766)
-- Name: ack_status_pkey; Type: CONSTRAINT; Schema: public; Owner: postgres; Tablespace: 
--

ALTER TABLE ONLY ack_status
    ADD CONSTRAINT ack_status_pkey PRIMARY KEY (id);


--
-- TOC entry 2857 (class 2606 OID 31153)
-- Name: acknowledgment_request_id_receiver_key; Type: CONSTRAINT; Schema: public; Owner: postgres; Tablespace: 
--

ALTER TABLE ONLY acknowledgment
    ADD CONSTRAINT acknowledgment_request_id_receiver_key UNIQUE (request_id, receiver);


--
-- TOC entry 2871 (class 2606 OID 22778)
-- Name: address_pkey; Type: CONSTRAINT; Schema: public; Owner: postgres; Tablespace: 
--

ALTER TABLE ONLY address
    ADD CONSTRAINT address_pkey PRIMARY KEY (id);


--
-- TOC entry 2867 (class 2606 OID 22758)
-- Name: person_pkey; Type: CONSTRAINT; Schema: public; Owner: postgres; Tablespace: 
--

ALTER TABLE ONLY person
    ADD CONSTRAINT person_pkey PRIMARY KEY (id);


--
-- TOC entry 2863 (class 2606 OID 22742)
-- Name: request_status_pkey; Type: CONSTRAINT; Schema: public; Owner: postgres; Tablespace: 
--

ALTER TABLE ONLY request_status
    ADD CONSTRAINT request_status_pkey PRIMARY KEY (id);


--
-- TOC entry 2865 (class 2606 OID 22750)
-- Name: table_name_pkey; Type: CONSTRAINT; Schema: public; Owner: postgres; Tablespace: 
--

ALTER TABLE ONLY table_name
    ADD CONSTRAINT table_name_pkey PRIMARY KEY (id);


--
-- TOC entry 2861 (class 2606 OID 22734)
-- Name: type_pkey; Type: CONSTRAINT; Schema: public; Owner: postgres; Tablespace: 
--

ALTER TABLE ONLY request_type
    ADD CONSTRAINT type_pkey PRIMARY KEY (id);


--
-- TOC entry 2859 (class 2606 OID 22721)
-- Name: ‫‪acknowledgement_pkey; Type: CONSTRAINT; Schema: public; Owner: postgres; Tablespace: 
--

ALTER TABLE ONLY acknowledgment
    ADD CONSTRAINT "‫‪acknowledgement_pkey" PRIMARY KEY (id);


--
-- TOC entry 2855 (class 2606 OID 22713)
-- Name: ‫‪request‬‬_pkey; Type: CONSTRAINT; Schema: public; Owner: postgres; Tablespace: 
--

ALTER TABLE ONLY request
    ADD CONSTRAINT "‫‪request‬‬_pkey" PRIMARY KEY (id);


--
-- TOC entry 2874 (class 2606 OID 31026)
-- Name: request_status_fkey; Type: FK CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY request
    ADD CONSTRAINT request_status_fkey FOREIGN KEY (status) REFERENCES request_status(id) NOT VALID;


--
-- TOC entry 2872 (class 2606 OID 31002)
-- Name: request_table_name_fkey; Type: FK CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY request
    ADD CONSTRAINT request_table_name_fkey FOREIGN KEY (table_name) REFERENCES table_name(id) NOT VALID;


--
-- TOC entry 2873 (class 2606 OID 31014)
-- Name: request_type_fkey; Type: FK CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY request
    ADD CONSTRAINT request_type_fkey FOREIGN KEY (type) REFERENCES request_type(id) NOT VALID;


--
-- TOC entry 2875 (class 2606 OID 22722)
-- Name: ‫‪acknowledgement_request_id_fkey; Type: FK CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY acknowledgment
    ADD CONSTRAINT "‫‪acknowledgement_request_id_fkey" FOREIGN KEY (request_id) REFERENCES request(id);


--
-- TOC entry 3007 (class 0 OID 0)
-- Dependencies: 5
-- Name: public; Type: ACL; Schema: -; Owner: postgres
--

REVOKE ALL ON SCHEMA public FROM PUBLIC;
REVOKE ALL ON SCHEMA public FROM postgres;
GRANT ALL ON SCHEMA public TO postgres;
GRANT ALL ON SCHEMA public TO PUBLIC;


-- Completed on 2022-05-14 17:24:32 +0430

--
-- PostgreSQL database dump complete
--

