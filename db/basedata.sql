--
-- PostgreSQL database dump
--

-- Dumped from database version 9.4.4
-- Dumped by pg_dump version 9.4.4
-- Started on 2022-05-01 14:57:19 +0430

SET statement_timeout = 0;
SET lock_timeout = 0;
SET client_encoding = 'UTF8';
SET standard_conforming_strings = on;
SET check_function_bodies = false;
SET client_min_messages = warning;

--
-- TOC entry 176 (class 3079 OID 12777)
-- Name: plpgsql; Type: EXTENSION; Schema: -; Owner: -
--

CREATE EXTENSION IF NOT EXISTS plpgsql WITH SCHEMA pg_catalog;


--
-- TOC entry 2930 (class 0 OID 0)
-- Dependencies: 176
-- Name: EXTENSION plpgsql; Type: COMMENT; Schema: -; Owner: -
--

COMMENT ON EXTENSION plpgsql IS 'PL/pgSQL procedural language';


SET search_path = public, pg_catalog;

--
-- TOC entry 174 (class 1259 OID 22714)
-- Name: acknowledgement_id_seq; Type: SEQUENCE; Schema: public; Owner: -
--

CREATE SEQUENCE acknowledgement_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


SET default_with_oids = false;

--
-- TOC entry 175 (class 1259 OID 22716)
-- Name: acknowledgment; Type: TABLE; Schema: public; Owner: -
--

CREATE TABLE acknowledgment (
    id bigint DEFAULT nextval('acknowledgement_id_seq'::regclass) NOT NULL,
    request_id bigint,
    receiver integer,
    ack smallint
);


--
-- TOC entry 172 (class 1259 OID 22703)
-- Name: request_id_seq; Type: SEQUENCE; Schema: public; Owner: -
--

CREATE SEQUENCE request_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


--
-- TOC entry 173 (class 1259 OID 22705)
-- Name: request; Type: TABLE; Schema: public; Owner: -
--

CREATE TABLE request (
    id bigint DEFAULT nextval('request_id_seq'::regclass) NOT NULL,
    table_id bigint,
    table_name_id integer,
    applicant integer,
    reviewer integer,
    type smallint,
    status smallint,
    data bytea,
    description json,
    created_at bigint
);


--
-- TOC entry 2813 (class 2606 OID 22721)
-- Name: ‫‪acknowledgement_pkey; Type: CONSTRAINT; Schema: public; Owner: -
--

ALTER TABLE ONLY acknowledgment
    ADD CONSTRAINT "‫‪acknowledgement_pkey" PRIMARY KEY (id);


--
-- TOC entry 2811 (class 2606 OID 22713)
-- Name: ‫‪request‬‬_pkey; Type: CONSTRAINT; Schema: public; Owner: -
--

ALTER TABLE ONLY request
    ADD CONSTRAINT "‫‪request‬‬_pkey" PRIMARY KEY (id);


--
-- TOC entry 2814 (class 2606 OID 22722)
-- Name: ‫‪acknowledgement_request_id_fkey; Type: FK CONSTRAINT; Schema: public; Owner: -
--

ALTER TABLE ONLY acknowledgment
    ADD CONSTRAINT "‫‪acknowledgement_request_id_fkey" FOREIGN KEY (request_id) REFERENCES request(id);


-- Completed on 2022-05-01 14:57:19 +0430

--
-- PostgreSQL database dump complete
--

