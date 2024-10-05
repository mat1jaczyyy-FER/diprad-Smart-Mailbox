/****** Object:  Table [dbo].[mailbox]    Script Date: 10/5/2024 6:23:52 PM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[mailbox](
	[ID] [int] IDENTITY(1,1) NOT NULL,
	[public_code] [nchar](7) NOT NULL,
	[private_code] [nchar](7) NOT NULL,
	[IDtype] [int] NOT NULL,
	[config] [nvarchar](50) NULL,
	[last_seen] [datetime] NULL,
	[battery_low] [bit] NOT NULL,
	[last_push] [datetime] NULL,
 CONSTRAINT [PK_mailbox] PRIMARY KEY CLUSTERED 
(
	[ID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[mailbox_type]    Script Date: 10/5/2024 6:23:52 PM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[mailbox_type](
	[ID] [int] IDENTITY(1,1) NOT NULL,
	[name] [nvarchar](10) NOT NULL,
 CONSTRAINT [PK_mailbox_type] PRIMARY KEY CLUSTERED 
(
	[ID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[push]    Script Date: 10/5/2024 6:23:52 PM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[push](
	[ID] [int] IDENTITY(1,1) NOT NULL,
	[subscription] [nvarchar](max) NOT NULL,
	[expired] [bit] NOT NULL,
 CONSTRAINT [PK_push] PRIMARY KEY CLUSTERED 
(
	[ID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY] TEXTIMAGE_ON [PRIMARY]
GO
/****** Object:  Table [dbo].[registrar]    Script Date: 10/5/2024 6:23:52 PM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[registrar](
	[ID] [int] IDENTITY(1,1) NOT NULL,
	[IDmailbox] [int] NOT NULL,
	[IDuser] [int] NOT NULL,
	[removed] [bit] NOT NULL,
 CONSTRAINT [PK_registra] PRIMARY KEY CLUSTERED 
(
	[ID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[subscription]    Script Date: 10/5/2024 6:23:52 PM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[subscription](
	[ID] [int] IDENTITY(1,1) NOT NULL,
	[IDregistrar] [int] NOT NULL,
	[IDpush] [int] NOT NULL,
	[enabled] [bit] NOT NULL,
 CONSTRAINT [PK_subscription] PRIMARY KEY CLUSTERED 
(
	[ID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[user]    Script Date: 10/5/2024 6:23:52 PM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[user](
	[ID] [int] IDENTITY(1,1) NOT NULL,
	[username] [nvarchar](30) NOT NULL,
	[salt] [binary](16) NOT NULL,
	[hash] [binary](64) NOT NULL,
 CONSTRAINT [PK_user] PRIMARY KEY CLUSTERED 
(
	[ID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
SET IDENTITY_INSERT [dbo].[mailbox_type] ON 
GO
INSERT [dbo].[mailbox_type] ([ID], [name]) VALUES (1, N'ultrasonic')
GO
INSERT [dbo].[mailbox_type] ([ID], [name]) VALUES (2, N'infrared')
GO
SET IDENTITY_INSERT [dbo].[mailbox_type] OFF
GO
ALTER TABLE [dbo].[mailbox] ADD  CONSTRAINT [DF_mailbox_battery_low]  DEFAULT ((0)) FOR [battery_low]
GO
ALTER TABLE [dbo].[push] ADD  CONSTRAINT [DF_push_expired]  DEFAULT ((0)) FOR [expired]
GO
ALTER TABLE [dbo].[registrar] ADD  CONSTRAINT [DF_registrar_removed]  DEFAULT ((0)) FOR [removed]
GO
ALTER TABLE [dbo].[subscription] ADD  CONSTRAINT [DF_subscription_enabled]  DEFAULT ((0)) FOR [enabled]
GO
ALTER TABLE [dbo].[mailbox]  WITH CHECK ADD  CONSTRAINT [FK_mailbox_mailbox_type] FOREIGN KEY([IDtype])
REFERENCES [dbo].[mailbox_type] ([ID])
GO
ALTER TABLE [dbo].[mailbox] CHECK CONSTRAINT [FK_mailbox_mailbox_type]
GO
ALTER TABLE [dbo].[registrar]  WITH CHECK ADD  CONSTRAINT [FK_registrar_mailbox] FOREIGN KEY([IDmailbox])
REFERENCES [dbo].[mailbox] ([ID])
GO
ALTER TABLE [dbo].[registrar] CHECK CONSTRAINT [FK_registrar_mailbox]
GO
ALTER TABLE [dbo].[registrar]  WITH CHECK ADD  CONSTRAINT [FK_registrar_user] FOREIGN KEY([IDuser])
REFERENCES [dbo].[user] ([ID])
GO
ALTER TABLE [dbo].[registrar] CHECK CONSTRAINT [FK_registrar_user]
GO
ALTER TABLE [dbo].[subscription]  WITH CHECK ADD  CONSTRAINT [FK_subscription_push] FOREIGN KEY([IDpush])
REFERENCES [dbo].[push] ([ID])
GO
ALTER TABLE [dbo].[subscription] CHECK CONSTRAINT [FK_subscription_push]
GO
ALTER TABLE [dbo].[subscription]  WITH CHECK ADD  CONSTRAINT [FK_subscription_registrar] FOREIGN KEY([IDregistrar])
REFERENCES [dbo].[registrar] ([ID])
GO
ALTER TABLE [dbo].[subscription] CHECK CONSTRAINT [FK_subscription_registrar]
GO
/****** Object:  StoredProcedure [dbo].[sp_mailbox_battery]    Script Date: 10/5/2024 6:23:56 PM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO

CREATE PROCEDURE [dbo].[sp_mailbox_battery]
	@private_code NCHAR(7),
	@low BIT
AS
BEGIN
	UPDATE dbo.mailbox
	SET
		last_seen = GETUTCDATE(),
		battery_low = @low
	WHERE
		private_code = @private_code;

	SELECT
		p.ID AS IDpush,
		p.subscription,
		m.public_code,
		m.last_seen
	FROM
		dbo.subscription s
	JOIN
		dbo.push p ON p.ID = s.IDpush
	JOIN
		dbo.registrar r ON r.ID = s.IDregistrar
	JOIN
		dbo.mailbox m ON m.ID = r.IDmailbox
	WHERE
		m.private_code = @private_code AND
		s.enabled = 1 AND
		p.expired = 0 AND
		r.removed = 0
END
GO
/****** Object:  StoredProcedure [dbo].[sp_mailbox_config]    Script Date: 10/5/2024 6:23:56 PM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO

CREATE PROCEDURE [dbo].[sp_mailbox_config]
	@private_code NCHAR(7)
AS
BEGIN
	UPDATE dbo.mailbox
	SET last_seen = GETUTCDATE()
	WHERE private_code = @private_code;

	SELECT IDtype, config
	FROM dbo.mailbox
	WHERE private_code = @private_code;
END
GO
/****** Object:  StoredProcedure [dbo].[sp_mailbox_notify]    Script Date: 10/5/2024 6:23:56 PM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO

CREATE PROCEDURE [dbo].[sp_mailbox_notify]
	@private_code NCHAR(7)
AS
BEGIN
	UPDATE dbo.mailbox
	SET
		last_seen = GETUTCDATE(),
		last_push = GETUTCDATE()
	WHERE
		private_code = @private_code;

	SELECT
		p.ID AS IDpush,
		p.subscription,
		m.public_code,
		m.last_push
	FROM
		dbo.subscription s
	JOIN
		dbo.push p ON p.ID = s.IDpush
	JOIN
		dbo.registrar r ON r.ID = s.IDregistrar
	JOIN
		dbo.mailbox m ON m.ID = r.IDmailbox
	WHERE
		m.private_code = @private_code AND
		s.enabled = 1 AND
		p.expired = 0 AND
		r.removed = 0
END
GO
/****** Object:  StoredProcedure [dbo].[sp_push_exists]    Script Date: 10/5/2024 6:23:56 PM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO

CREATE PROCEDURE [dbo].[sp_push_exists]
	@subscription NVARCHAR(MAX)
AS
SELECT ISNULL(
	(
		SELECT TOP 1 ID 
        FROM dbo.push 
        WHERE
			expired = 0 AND
			subscription = @subscription
	), 
	0
) AS result
GO
/****** Object:  StoredProcedure [dbo].[sp_push_expire]    Script Date: 10/5/2024 6:23:56 PM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO

CREATE PROCEDURE [dbo].[sp_push_expire]
	@IDpush INT
AS
UPDATE dbo.push
SET expired = 1
WHERE ID = @IDpush;
GO
/****** Object:  StoredProcedure [dbo].[sp_push_save]    Script Date: 10/5/2024 6:23:56 PM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO

CREATE PROCEDURE [dbo].[sp_push_save]
	@subscription NVARCHAR(MAX)
AS
BEGIN
	INSERT INTO dbo.push (subscription)
	VALUES (@subscription);

	SELECT SCOPE_IDENTITY() AS result;
END
GO
/****** Object:  StoredProcedure [dbo].[sp_registrar_add]    Script Date: 10/5/2024 6:23:56 PM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO

CREATE PROCEDURE [dbo].[sp_registrar_add]
	@IDuser INT,
	@public_code NCHAR(7)
AS
BEGIN
	IF NOT EXISTS (SELECT 1 FROM dbo.[user] WHERE ID = @IDuser)
	BEGIN
		SELECT 0 AS result;
		RETURN;
	END;

	DECLARE @IDmailbox INT = (SELECT ID FROM dbo.mailbox WHERE public_code = @public_code);

	IF @IDmailbox IS NULL
	BEGIN
		SELECT 0 AS result;
		RETURN;
	END;

	DECLARE @IDregistrar INT = (SELECT ID FROM dbo.registrar WHERE IDmailbox = @IDmailbox AND IDuser = @IDuser AND removed = 0)

	IF @IDregistrar IS NOT NULL 
    BEGIN
        SELECT @IDregistrar AS result;
        RETURN;
    END;

	INSERT INTO dbo.registrar (IDmailbox, IDuser)
	VALUES (@IDmailbox, @IDuser);

	SELECT SCOPE_IDENTITY() AS result;
END
GO
/****** Object:  StoredProcedure [dbo].[sp_registrar_get]    Script Date: 10/5/2024 6:23:56 PM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO

CREATE PROCEDURE [dbo].[sp_registrar_get]
	@IDuser INT
AS
SELECT
	m.public_code,
	m.last_seen,
	m.battery_low,
	m.last_push
FROM
	dbo.registrar r
JOIN
	dbo.mailbox m ON m.ID = r.IDmailbox
WHERE
	r.IDuser = @IDuser AND
	r.removed = 0
GO
/****** Object:  StoredProcedure [dbo].[sp_registrar_remove]    Script Date: 10/5/2024 6:23:56 PM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO

CREATE PROCEDURE [dbo].[sp_registrar_remove]
	@IDuser INT,
	@public_code NCHAR(7)
AS
BEGIN
	IF NOT EXISTS (SELECT 1 FROM dbo.[user] WHERE ID = @IDuser)
	BEGIN
		SELECT 0 AS result;
		RETURN;
	END;

	DECLARE @IDmailbox INT = (SELECT ID FROM dbo.mailbox WHERE public_code = @public_code);

	IF @IDmailbox IS NULL
	BEGIN
		SELECT 0 AS result;
		RETURN;
	END;

	DECLARE @IDregistrar INT = (SELECT ID FROM dbo.registrar WHERE IDmailbox = @IDmailbox AND IDuser = @IDuser AND removed = 0)

	IF @IDregistrar IS NULL 
    BEGIN
		SELECT 0 AS result;
		RETURN;
    END;

	UPDATE dbo.registrar
	SET removed = 1
	WHERE ID = @IDregistrar

	SELECT @IDregistrar AS result;
END
GO
/****** Object:  StoredProcedure [dbo].[sp_subscription_change]    Script Date: 10/5/2024 6:23:56 PM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO

CREATE PROCEDURE [dbo].[sp_subscription_change]
	@IDuser INT,
	@public_code NCHAR(7),
	@IDpush INT,
	@enabled BIT
AS
BEGIN
	IF NOT EXISTS (SELECT 1 FROM dbo.[user] WHERE ID = @IDuser)
	BEGIN
		SELECT 0 AS result;
		RETURN;
	END;

	DECLARE @IDmailbox INT = (SELECT ID FROM dbo.mailbox WHERE public_code = @public_code);

	IF @IDmailbox IS NULL
	BEGIN
		SELECT 0 AS result;
		RETURN;
	END;

	DECLARE @IDregistrar INT = (SELECT ID FROM dbo.registrar WHERE IDmailbox = @IDmailbox AND IDuser = @IDuser AND removed = 0)

	IF @IDregistrar IS NULL 
    BEGIN
		SELECT 0 AS result;
        RETURN;
    END;

	IF NOT EXISTS (SELECT 1 FROM dbo.push WHERE ID = @IDpush)
	BEGIN
		SELECT 0 AS result;
		RETURN;
	END;

	DECLARE @IDsubscription INT = (SELECT ID FROM dbo.subscription WHERE IDregistrar = @IDregistrar AND IDpush = @IDpush);

	IF @IDsubscription IS NULL 
    BEGIN
		INSERT INTO dbo.subscription (IDregistrar, IDpush, enabled)
		VALUES (@IDregistrar, @IDpush, @enabled)

		SELECT SCOPE_IDENTITY() AS result;
        RETURN;
    END;

	UPDATE dbo.subscription
	SET enabled = @enabled
	WHERE ID = @IDsubscription;

	SELECT @IDsubscription AS result;
END
GO
/****** Object:  StoredProcedure [dbo].[sp_subscription_get]    Script Date: 10/5/2024 6:23:56 PM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO

CREATE PROCEDURE [dbo].[sp_subscription_get]
	@IDuser INT,
	@IDpush INT NULL
AS
SELECT
	m.public_code
FROM
	dbo.subscription s
JOIN
	dbo.push p ON p.ID = s.IDpush
JOIN
	dbo.registrar r ON r.ID = s.IDregistrar
JOIN
	dbo.mailbox m ON m.ID = r.IDmailbox
WHERE
	r.IDuser = @IDuser AND
	r.removed = 0 AND
	s.enabled = 1 AND
	p.ID = @IDpush AND
	p.expired = 0
GO
/****** Object:  StoredProcedure [dbo].[sp_user_login]    Script Date: 10/5/2024 6:23:56 PM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO

CREATE PROCEDURE [dbo].[sp_user_login]
	@username NVARCHAR(30)
AS
SELECT
	ID, salt, hash
FROM
	dbo.[user]
WHERE
	username = @username;
GO
/****** Object:  StoredProcedure [dbo].[sp_user_register]    Script Date: 10/5/2024 6:23:56 PM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO

CREATE PROCEDURE [dbo].[sp_user_register]
	@username NVARCHAR(30),
	@salt BINARY(16),
	@hash BINARY(64)
AS
BEGIN
	DECLARE @IDuser INT = (SELECT ID FROM dbo.[user] WHERE username = @username);

	IF @IDuser IS NOT NULL
	BEGIN
		SELECT 0 AS result;
		RETURN;
	END;
	
	INSERT INTO dbo.[user] (username, salt, hash)
	VALUES (@username, @salt, @hash);

	SELECT SCOPE_IDENTITY() AS result;
END
GO
