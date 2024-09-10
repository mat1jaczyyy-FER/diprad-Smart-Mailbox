const rtf = new Intl.RelativeTimeFormat('en', { style: 'short' });

self.addEventListener('push', e => {
    const { public_code, last_push } = e.data?.json()?? {};

    e.waitUntil(self.registration.showNotification(
        "Mail has arrived – Smart Mailbox",
        {
            body: public_code
                ? `Mail has just arrived to your Smart Mailbox ${public_code}.`
                : 'Mail has just arrived to one of your Smart Mailboxes.',
            badge: '/res/favicon.svg',
            icon: '/res/favicon.svg',
            renotify: true,
            tag: public_code,
            timestamp: Date.parse(last_push? last_push: new Date())
        }
    ));
});

self.addEventListener('notificationclick', e => {
    e.notification.close();
});
